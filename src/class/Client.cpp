#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"
#include "class/Channel.hpp"

#include <sys/types.h>
#include <sys/socket.h>

#include <iomanip>
#include <sstream>
#include <algorithm>

const std::string Client::create_cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg, const std::string &message)
{
	std::ostringstream oss;
	oss << ':' << prefix << ' ' << cmd;

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty())
		oss << " :" << message;

	return _create_line(oss.str());
}

bool Client::is_valid_nickname(const std::string &nickname)
{
	if (std::isdigit(nickname[0]) || nickname[0] == '-')
		return false;

	for (std::string::const_iterator it = nickname.begin(); it != nickname.end(); ++it)
		if (!std::isalnum(*it) && std::string("_-[]{}\\`^|").find(*it) == std::string::npos)
			return false;

	return true;
}

Client::Client(const int fd, const std::string ip, Server &server):
	_registered(false),
	_fd(fd),
	_ip(ip),
	_disconnect_request(false),
	_server(server),
	_quit_reason("Client closed connection")
{
	log("Accepted connection from " + std::string(_ip));
}

Client::~Client()
{
	close(_fd);
	log("Closed connection");
}

const std::string Client::create_motd_reply() const
{
	const std::vector<std::string> &motd_lines = _server.get_motd_lines();

	if (motd_lines.empty())
		return create_reply(ERR_NOMOTD, "", "MOTD File is missing");

	std::string reply = create_reply(RPL_MOTDSTART, "", "- " + _server.get_name() + " message of the day");

	for (std::vector<std::string>::const_iterator it = motd_lines.begin(); it != motd_lines.end(); ++it)
		reply += create_reply(RPL_MOTD, "", "- " + *it);

	reply += create_reply(RPL_ENDOFMOTD, "", "End of MOTD command");

	return reply;
}

const std::string Client::create_reply(reply_code code, const std::string &arg, const std::string &message, const bool colon) const
{
	std::ostringstream oss;

	oss << ':' << _server.get_name() << ' ' << std::setfill('0') << std::setw(3) << code << ' ' << get_nickname(false);

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty() || colon)
		oss << " :";
	oss << message;

	return _create_line(oss.str());
}

const std::string Client::generate_who_reply(const std::string &context) const
{
	std::ostringstream oss;

	std::string status_flags = "H";
	if (context != "*" && std::find(_channel_operator.begin(), _channel_operator.end(), context) != _channel_operator.end())
		status_flags += "@";

	return context + ' ' + _get_username() + ' ' + _ip + ' ' + _server.get_name() + ' ' + _nickname + ' ' + status_flags;
}

ssize_t Client::send(const std::string &message)
{
	log("Sending message: " + message, debug);

	ssize_t bytes_sent = ::send(_fd, message.c_str(), message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1) {
		log("Failed to send message", error);
		if (!_disconnect_request)
			send_error("Internal server error");
	}

	return bytes_sent;
}

void Client::broadcast_quit() const
{
	broadcast(create_cmd_reply(get_mask(), "QUIT", "", _quit_reason));
}

void Client::broadcast(const std::string &message) const
{
	clients_t clients_to_broadcast;

	for (channels_t::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
		const Channel *channel = it->second;

		clients_t members = channel->get_members();
		for (clients_t::iterator it = members.begin(); it != members.end(); ++it) {
			Client *client = it->second;
			clients_to_broadcast[client->get_fd()] = client;
		}
	}

	clients_to_broadcast.erase(_fd);

	for (clients_t::iterator it = clients_to_broadcast.begin(); it != clients_to_broadcast.end(); ++it) {
		Client &client = *it->second;
		client.send(message);
	}
}

void Client::cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg, const std::string &message)
{
	send(create_cmd_reply(prefix, cmd, arg, message));
}

void Client::handle_messages(const std::string &messages)
{
	log("Received messages:\n" + messages, debug);

	_buffer += messages;
	size_t pos;
	while ((pos = _buffer.find('\n')) != std::string::npos)
	{
		_handle_message(_buffer.substr(0, pos));
		_buffer.erase(0, pos + 1);
	}
}

void Client::log(const std::string &message, const log_level level) const
{
	if (_server.is_verbose() || level != debug)
		::log("Client " + to_string(_fd), message, level);
}

void Client::reply(reply_code code, const std::string &arg, const std::string &message)
{
	send(create_reply(code, arg, message));
}

void Client::send_error(const std::string &message)
{
	std::ostringstream oss;

	oss << "ERROR :Closing connection: " << get_nickname(false) << '[' << _get_username() << '@' << _ip << ']';
	if (!message.empty())
		oss << " (" << message << ')';

	_disconnect_request = true;
	send(_create_line(oss.str()));
}

void Client::join(const std::string &original_channel_name, Channel &channel, const std::string &passkey)
{
	if (channel.is_client_member(*this))
		return;

	if (channel.is_full())
		reply(ERR_CHANNELISFULL, channel.get_name(), "Cannot join channel (+l) -- Channel is full, try later");

	else if (!channel.check_passkey(passkey))
		reply(ERR_BADCHANNELKEY, channel.get_name(), "Cannot join channel (+k) -- Wrong channel key");

	else if (_channels.size() >= Client::_max_channels)
		reply(ERR_TOOMANYCHANNELS, channel.get_name(), "You have joined too many channels");

	else if (!channel.is_client_invited(*this) && channel.is_invite_only())
		reply(ERR_INVITEONLYCHAN, channel.get_name(), "Cannot join channel (+i) -- Invited users only");

	else {
		channel.add_client(*this);

		const std::string &channel_name = channel.get_name();
		_channels[channel_name] = &channel;

		const std::string &reply = create_cmd_reply(get_mask(), "JOIN", "", original_channel_name);
		channel.broadcast(reply);
		names(channel_name);
	}
}

void Client::kick(const std::string &nick_to_kick, Channel &channel, const std::string &reason)
{
	Server &server = get_server();
	Client *client_to_kick = server.get_client(nick_to_kick);

	if (!channel.is_client_member(*this))
		reply(ERR_NOTONCHANNEL, channel.get_name(), "You're not on that channel");

	else if (!is_channel_operator(channel.get_name()))
		reply(ERR_CHANOPRIVSNEEDED, channel.get_name(), "You're not channel operator");

	else if (!client_to_kick)
		reply(ERR_NOSUCHNICK, channel.get_name(), "No such nick/channel");

	else if (!channel.is_client_member(*client_to_kick))
		reply(ERR_USERNOTINCHANNEL, channel.get_name(), "They aren't on that channel");

	else {
		channel.broadcast(create_cmd_reply(
			get_mask(), "KICK", channel.get_name() + ' ' + nick_to_kick, reason
		));
		channel.remove_client(*client_to_kick);
		client_to_kick->delete_channel(channel);
	}
}

void Client::names(const std::string &channel_name)
{
	args_t args;
	args.push_back("NAMES");
	args.push_back(channel_name);
	Command::execute(args, *this, _server);
}

void Client::part(Channel &channel, const std::string &reason)
{
	channel.broadcast(create_cmd_reply(
		get_mask(), "PART", channel.get_name() , reason
	));
	channel.remove_client(*this);
	_channels.erase(channel.get_name());
	remove_channel_operator(channel.get_name());

	if (channel.get_members().empty())
		_server.delete_channel(channel.get_name());
}

const std::string Client::get_nickname(bool allow_empty) const
{
	if (!allow_empty && _nickname.empty())
		return "*";

	return _nickname;
}

bool Client::is_registered() const
{
	return _registered;
}

std::string Client::get_mask() const
{
	return std::string(_nickname + "!" + _get_username() + "@" + _ip);
}

std::string Client::get_realname() const
{
	return _realname;
}

const channels_t &Client::get_channels() const
{
	return _channels;
}

bool Client::is_channel_operator(std::string channel_name) const
{
	const std::string &lower_channel_name = to_lower(channel_name);

	for (std::vector<std::string>::const_iterator it = _channel_operator.begin(); it != _channel_operator.end(); ++it)
		if (to_lower(*it) == lower_channel_name)
			return true;

	return false;
}

bool Client::has_disconnect_request() const
{
	return _disconnect_request;
}

int Client::get_fd() const
{
	return _fd;
}

Server &Client::get_server() const
{
	return _server;
}

void Client::set_nickname(const std::string &nickname)
{
	if (nickname.size() > _max_nickname_size)
		return reply(ERR_ERRONEUSNICKNAME, nickname, "Nickname too long, max. 9 characters");

	if (!is_valid_nickname(nickname))
		return reply(ERR_ERRONEUSNICKNAME, nickname, "Erroneous nickname");

	if (_server.get_client(nickname) != NULL)
		return reply(ERR_NICKNAMEINUSE, nickname, "Nickname is already in use");

	if (_registered) {
		const std::string cmd_reply = create_cmd_reply(get_mask(), "NICK", "", nickname);
		send(cmd_reply);
		broadcast(cmd_reply);
	}

	_nickname = nickname;

	if (!_registered)
		_check_registration();
}

void Client::set_realname(const std::string &realname)
{
	_realname = realname;

	if (_realname.size() > _max_realname_len)
		_realname.resize(_max_realname_len);
}

void Client::set_username(const std::string &username)
{
	if (!_is_valid_username(username))
		return send_error("Invalid user name");

	_username = username;

	if (_username.size() > _max_username_len)
		_username.resize(_max_username_len);

	_check_registration();
}

void Client::delete_channel(const Channel &channel)
{
	_channels.erase(channel.get_name());
}

void Client::remove_channel_operator(const std::string &channel)
{
	std::vector<std::string>::iterator it = std::find(_channel_operator.begin(), _channel_operator.end(), channel);

	if (it != _channel_operator.end())
		_channel_operator.erase(it);
}

void Client::set_channel_operator(const std::string &channel)
{
	if (!is_channel_operator(channel))
		_channel_operator.push_back(channel);
}

void Client::set_password(const std::string &password)
{
	_password = password;
}

void Client::set_quit_reason(const std::string &reason)
{
	_quit_reason = reason;
}

bool Client::_is_valid_username(const std::string &username)
{
	for (std::string::const_iterator it = username.begin(); it != username.end(); ++it)
		if (!std::isalnum(*it) && std::string("_-").find(*it) == std::string::npos)
			return false;

	return true;
}

std::string Client::_create_line(const std::string &content)
{
	std::string line = content;

	if (line.size() > _max_message_size - 2) {
		line.erase(_max_message_size - 7);
		line += "[CUT]";
	}
	line += "\r\n";

	return line;
}

void Client::_check_registration()
{
	if (_nickname.empty() || _username.empty())
		return;

	if (!_server.check_password(_password))
		return send_error("Access denied: Bad password?");

	_registered = true;
	_server.register_client();

	_greet();
}

void Client::_greet()
{
	std::string chanlimit = to_string(Client::_max_channels);
	std::string channellen = to_string(Channel::max_channel_name_size);
	std::string kicklen = to_string(Client::max_kick_reason_len);
	std::string nicklen = to_string(Client::_max_nickname_size);
	std::string topiclen = to_string(Channel::max_topic_len);

	std::string channels_count = to_string(_server.get_channels().size());
	std::string clients_count = to_string(_server.get_clients().size());
	std::string max_clients = to_string(_server.get_max_clients());
	std::string max_registred_clients = to_string(_server.get_max_registered_clients());
	std::string registred_clients_count = to_string(_server.get_registered_clients_count());

	std::string reply = // https://modern.ircdocs.horse/#rplwelcome-001
		create_reply(RPL_WELCOME, "", "Welcome to the Internet Relay Network " + get_mask())
		+ create_reply(RPL_YOURHOST, "", "Your host is " + _server.get_name() + ", running version " VERSION)
		+ create_reply(RPL_CREATED, "", "This server has been started " + _server.get_start_time())
		+ create_reply(RPL_MYINFO, _server.get_name() + " " VERSION " o iklt")
		+ create_reply(RPL_ISUPPORT, "RFC2812 IRCD=ft_irc CHARSET=UTF-8 CASEMAPPING=ascii PREFIX=(o)@ CHANTYPES=#& CHANMODES=,k,l,it", "are supported on this server")
		+ create_reply(RPL_ISUPPORT, "CHANLIMIT=#&:" + chanlimit + " CHANNELLEN=" + channellen + " NICKLEN=" + nicklen + " TOPICLEN=" + topiclen + " KICKLEN=" + kicklen, "are supported on this server")
		+ create_reply(RPL_LUSERCLIENT, "", "There are " + registred_clients_count + " users and 0 services on 1 servers")
		+ create_reply(RPL_LUSERCHANNELS, channels_count, "channels formed")
		+ create_reply(RPL_LUSERME, "", "I have " + registred_clients_count + " users, 0 services and 0 servers")
		+ create_reply(RPL_LOCALUSERS, registred_clients_count + ' ' + max_registred_clients, "Current local users: " + registred_clients_count + ", Max: " + max_registred_clients)
		+ create_reply(RPL_LOCALUSERS, registred_clients_count + ' ' + max_registred_clients, "Current global users: " + registred_clients_count + ", Max: " + max_registred_clients)
		+ create_reply(RPL_STATSDLINE, "", "Highest connection count: " + max_clients + " (" + clients_count + " connections received)")
		+ create_motd_reply();

	send(reply);
}

void Client::_handle_message(std::string message)
{
	if (message.empty())
		return;

	if (message.size() > _max_message_size - 1)
		send_error("Request too long");

	if (message[message.size() - 1] == '\r')
		message.erase(message.size() - 1);

	args_t args;
	size_t pos;
	while ((pos = message.find(' ')) != std::string::npos && message.find(':') != 0)
	{
		if (pos > 0)
			args.push_back(message.substr(0, pos));
		message.erase(0, pos + 1);
	}
	if (!message.empty())
	{
		if (message[0] == ':')
			message.erase(0, 1);
		args.push_back(message);
	}

	std::ostringstream oss;
	for (args_t::iterator it = args.begin(); it != args.end(); ++it) {
		oss << '"' << *it << "\"";
		if (it + 1 != args.end())
			oss << ", ";
	}
	log("Parsed command: " + oss.str(), debug);

	Command::execute(args, *this, _server);
}

std::string Client::_get_username() const
{
	return '~' + _username;
}
