#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"
#include "class/Channel.hpp"

#include <sys/types.h>
#include <sys/socket.h>

#include <iomanip>
#include <sstream>
#include <algorithm>

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

void Client::handle_messages(std::string messages)
{
	std::string debug_messages = messages;
	for (size_t pos = 0; (pos = debug_messages.find('\t', pos)) != std::string::npos; pos += 2)
		debug_messages.replace(pos, 1, "\\t");
	for (size_t pos = 0; (pos = debug_messages.find('\n', pos)) != std::string::npos; pos += 2)
		debug_messages.replace(pos, 1, "\\n");
	for (size_t pos = 0; (pos = debug_messages.find('\r', pos)) != std::string::npos; pos += 2)
		debug_messages.replace(pos, 1, "\\r");
	log("Received messages:\n" + debug_messages, debug);

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

ssize_t Client::send(const std::string &message) const
{
	ssize_t bytes_sent = ::send(_fd, message.c_str(), message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1)
		throw std::runtime_error("Failed to send message");

	return bytes_sent;
}

void Client::reply(reply_code code, const std::string &arg, const std::string &message) const
{
	send(create_reply(code, arg, message));
}

void Client::cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg, const std::string &message) const
{
	send(create_cmd_reply(prefix, cmd, arg, message));
}

void Client::send_error(const std::string &message)
{
	std::ostringstream oss;

	oss << "ERROR :Closing connection: " << get_nickname(false) << '[' << _get_username() << '@' << _ip << ']';
	if (!message.empty())
		oss << " (" << message << ')';

	send(_create_line(oss.str()));

	_disconnect_request = true;
}

void Client::broadcast(const std::string &message) const
{
	clients_t clients_to_notify;

	for (channels_t::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
		Channel *channel = it->second;

		clients_t members = channel->get_members();
		for (clients_t::iterator member = members.begin(); member != members.end(); ++member) {
			Client *member_client = member->second;
			clients_to_notify[member_client->get_fd()] = member_client;
		}
	}

	clients_to_notify.erase(_fd);

	for (clients_t::iterator it = clients_to_notify.begin(); it != clients_to_notify.end(); ++it)
		it->second->send(message);
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

const bool &Client::is_registered() const
{
	return _registered;
}

bool Client::is_channel_operator(std::string channel_name) const
{
	const std::string &lower_channel_name = to_lower(channel_name);

	for (std::vector<std::string>::const_iterator it = _channel_operator.begin(); it != _channel_operator.end(); ++it)
		if (to_lower(*it) == lower_channel_name)
			return true;

	return false;
}

const std::string &Client::get_nickname(bool allow_empty) const
{
	static const std::string empty_nick = "*";

	if (!allow_empty && _nickname.empty())
		return empty_nick;

	return _nickname;
}

const bool &Client::has_disconnect_request() const
{
	return _disconnect_request;
}

void	Client::set_channel_operator(std::string channel)
{
	if (!is_channel_operator(channel))
		_channel_operator.push_back(channel);
}

void Client::remove_channel_operator(std::string channel)
{
	std::vector<std::string>::iterator it = std::find(_channel_operator.begin(), _channel_operator.end(), channel);

	if (it != _channel_operator.end())
		_channel_operator.erase(it);
}

void Client::set_nickname(const std::string &nickname)
{
	if (nickname.size() > _max_nickname_size)
		return reply(ERR_ERRONEUSNICKNAME, nickname, "Nickname too long, max. 9 characters");

	if (!_is_valid_nickname(nickname))
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

void Client::set_username(const std::string &username)
{
	if (!_is_valid_username(username))
		return send_error("Invalid user name");

	_username = username;

	if (_username.size() > _max_username_len)
		_username.resize(_max_username_len);

	_check_registration();
}

void Client::set_realname(const std::string &realname)
{
	_realname = realname;

	if (_realname.size() > _max_realname_len)
		_realname.resize(_max_realname_len);
}

void Client::set_password(const std::string &password)
{
	_password = password;
}

void Client::set_quit_reason(const std::string &reason)
{
	_quit_reason = reason;
}

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
	for (args_t::iterator it = args.begin(); it != args.end(); ++it)
	{
		oss << '"' << *it << "\"";
		if (it + 1 != args.end())
			oss << ", ";
	}
	log("Parsed command: " + oss.str(), debug);

	Command::execute(args, *this, _server);
}

std::string Client::create_reply(reply_code code, const std::string &arg, const std::string &message) const
{
	std::ostringstream oss;

	oss << ':' << _server.get_name() << ' ' << std::setfill('0') << std::setw(3) << code << ' ' << get_nickname(false);

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty())
		oss << " :" << message;

	return _create_line(oss.str());
}

void Client::_check_registration()
{
	if (_nickname.empty() || _username.empty())
		return ;

	if (!_server.check_password(_password))
		return send_error("Access denied: Bad password?");

	_registered = true;
	_server.register_client();

	_greet();
}

void Client::_greet() const
{
	std::string chanlimit = to_string(Client::_max_channels);
	std::string channellen = to_string(Channel::max_channel_name_size);
	std::string nicklen = to_string(Client::_max_nickname_size);
	std::string topiclen = to_string(Channel::max_topic_len);
	std::string kicklen = to_string(Client::_max_kick_message_len);

	std::string clients_count = to_string(_server.get_clients_count());
	std::string channels_count = to_string(_server.get_channels_count());
	std::string max_clients = to_string(_server.get_max_clients());
	std::string max_connections = to_string(_server.get_max_connections());
	std::string connections = to_string(_server.get_connections());

	std::string reply = // https://modern.ircdocs.horse/#rplwelcome-001
		create_reply(RPL_WELCOME, "", "Welcome to the Internet Relay Network " + get_mask())
		+ create_reply(RPL_YOURHOST, "", "Your host is " + _server.get_name() + ", running version " VERSION)
		+ create_reply(RPL_CREATED, "", "This server has been started " + _server.get_start_time())
		+ create_reply(RPL_MYINFO, _server.get_name() + " " VERSION " o iklt")
		+ create_reply(RPL_ISUPPORT, "RFC2812 IRCD=ft_irc CHARSET=UTF-8 CASEMAPPING=ascii PREFIX=(o)@ CHANTYPES=#& CHANMODES=,k,l,it", "are supported on this server")
		+ create_reply(RPL_ISUPPORT, "CHANLIMIT=#&:" + chanlimit + " CHANNELLEN=" + channellen + " NICKLEN=" + nicklen + " TOPICLEN=" + topiclen + " KICKLEN=" + kicklen, "are supported on this server")
		+ create_reply(RPL_LUSERCLIENT, "", "There are " + clients_count + " users and 0 services on 1 servers")
		+ create_reply(RPL_LUSERCHANNELS, channels_count, "channels formed")
		+ create_reply(RPL_LUSERME, "", "I have " + clients_count + " users, 0 services and 0 servers")
		+ create_reply(RPL_LOCALUSERS, clients_count + ' ' + max_clients, "Current local users: " + clients_count + ", Max: " + max_clients)
		+ create_reply(RPL_LOCALUSERS, clients_count + ' ' + max_clients, "Current global users: " + clients_count + ", Max: " + max_clients)
		+ create_reply(RPL_STATSDLINE, "", "Highest connection count: " + max_connections + " (" + connections + " connections received)")
		+ create_motd_reply();

	send(reply);
}

std::string Client::_get_username() const
{
	return '~' + _username;
}

bool Client::_is_valid_nickname(const std::string &nickname)
{
	if (std::isdigit(nickname[0]) || nickname[0] == '-')
		return false;

	for (std::string::const_iterator it = nickname.begin(); it != nickname.end(); ++it)
		if (!std::isalnum(*it) && std::string("_-[]{}\\`^|").find(*it) == std::string::npos)
			return false;

	return true;
}

bool Client::_is_valid_username(const std::string &username)
{
	for (std::string::const_iterator it = username.begin(); it != username.end(); ++it)
		if (!std::isalnum(*it) && std::string("_-").find(*it) == std::string::npos)
			return false;

	return true;
}

Server &Client::get_server( void ) const
{
	return _server;
}

const int &Client::get_fd( void )
{
	return _fd;
}

std::string Client::get_mask(void) const
{
	return std::string(_nickname + "!" + _get_username() + "@" + _ip);
}

const channels_t &Client::get_channels() const
{
	return _channels;
}

size_t Client::get_channels_count(void) const
{
	return _channels.size();
}

Channel *Client::get_channel(const std::string &name) const
{
	channels_t::const_iterator it = _channels.find(name);
	if (it == _channels.end())
		return NULL;

	return it->second;
}


bool	Client::join(Channel &channel, std::string passkey)
{
	if (channel.is_client_member(*this))
		return (true);

	if (!channel.is_client_invited(*this) && channel.is_invite_only())
		reply(ERR_INVITEONLYCHAN, channel.get_name(), "Cannot join channel (+i)");

	else if (channel.is_full())
		reply(ERR_CHANNELISFULL, channel.get_name(), "Cannot join channel (+l)");

	else if (!channel.check_passkey(passkey))
		reply(ERR_BADCHANNELKEY, channel.get_name(), "Cannot join channel (+k) - bad key");

	else if (channel.is_client_banned(*this))
		reply(ERR_BANNEDFROMCHAN, channel.get_name(), "Cannot join channel (+b)");

	else if (get_channels_count() >= Client::_max_channels)
		reply(ERR_TOOMANYCHANNELS, channel.get_name(), "You have joined too many channels");

	else
	{
		channel.add_client(*this);
		_channels[channel.get_name()] = &channel;

		return (true);
	}

	return (false);
}

void	Client::kick(Channel &channel, const std::string &kicked_client, std::string &reason)
{
	Server &server = get_server();
	Client *client_to_be_kicked = server.get_client(kicked_client);

	if (reason.size() > _max_kick_message_len)
		reason.resize(_max_kick_message_len);

	if (!channel.is_client_member(*this))
		reply(ERR_NOTONCHANNEL, channel.get_name(), "You're not on that channel");
	else if (!is_channel_operator(channel.get_name()))
		reply(ERR_CHANOPRIVSNEEDED, channel.get_name(), "You're not channel operator");
	else if (!client_to_be_kicked)
		reply(ERR_NOSUCHNICK, channel.get_name(), "No such nick/channel");
	else if (!channel.is_client_member(*client_to_be_kicked))
		reply(ERR_USERNOTINCHANNEL, channel.get_name(), "They aren't on that channel");
	else {
		channel.send_broadcast(create_cmd_reply(
			get_mask(), "KICK", channel.get_name() + ' ' + kicked_client, reason
		));
		channel.remove_client(*client_to_be_kicked);
		client_to_be_kicked->delete_channel(channel.get_name());
	}
}

void	Client::part(Channel &channel, std::string &reason)
{
	channel.send_broadcast(create_cmd_reply(
		get_mask(), "PART", channel.get_name() , reason
	));
	channel.remove_client(*this);
	_channels.erase(channel.get_name());
	remove_channel_operator(channel.get_name());

	if (channel.get_members().empty())
		_server.delete_channel(channel.get_name());
}

void	Client::close_all_channels(std::string &reason)
{
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		part(*it->second, reason);
}

void Client::delete_channel(const std::string &channel_name)
{
	Channel *channel = get_channel(channel_name);
	if (channel)
		_channels.erase(channel->get_name());
}

void Client::notify_quit() const
{
	broadcast(create_cmd_reply(get_mask(), "QUIT", "", _quit_reason));
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

std::string Client::get_realname( void ) const
{
	return _realname;
}

std::string Client::generate_who_reply(const std::string &context) const
{
	std::ostringstream oss;

	std::string status_flags = "H";
	if (context != "*" && std::find(_channel_operator.begin(), _channel_operator.end(), context) != _channel_operator.end())
		status_flags += "@";

	return context + ' ' + _get_username() + ' ' + _ip + ' ' + _server.get_name() + ' ' + _nickname + ' ' + status_flags;
}
