#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>

#include <iomanip>
#include <sstream>

Client::Client(const int fd, char *ip, Server &server) :
	_fd(fd),
	_ip(ip),
	_server(server),
	_disconnect_request(false),
	_registered(false)
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

void Client::reply(reply_code code, const std::string &arg, const std::string &message) const
{
	_send(_create_reply(code, arg, message));
}

void Client::send_error(const std::string &message)
{
	std::ostringstream oss;

	oss << "ERROR :Closing connection: " << get_nickname(false) << '[' << _get_username() << '@' << _ip << ']';
	if (!message.empty())
		oss << " (" << message << ')';

	_send(_create_line(oss.str()));

	_disconnect_request = true;
}

const bool &Client::is_registered() const
{
	return _registered;
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

void Client::set_nickname(const std::string &nickname)
{
	if (nickname.size() > _max_nickname_size)
		return reply(ERR_ERRONEUSNICKNAME, nickname, "Nickname too long, max. 9 characters");

	if (!_is_valid_nickname(nickname))
		return reply(ERR_ERRONEUSNICKNAME, nickname, "Erroneous nickname");

	if (_server.get_client(nickname) != NULL)
		return reply(ERR_NICKNAMEINUSE, nickname, "Nickname is already in use");

	_nickname = nickname;

	if (!_registered)
		_check_registration();
}

void Client::set_username(const std::string &username)
{
	if (_registered)
		return reply(ERR_ALREADYREGISTRED, "", "Connection already registered");

	if (!_is_valid_username(username))
		return send_error("Invalid user name");

	_username = username;

	_check_registration();
}

void Client::set_password(const std::string &password)
{
	if (!_username.empty() && !_nickname.empty())
		return reply(ERR_ALREADYREGISTRED, "", "Connection already registered");

	_password = password;
}

void Client::set_realname(const std::string &realname)
{
	_realname = realname;
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

	Command::execute(args, *this);
}

ssize_t Client::_send(const std::string &message) const
{
	ssize_t bytes_sent = send(_fd, message.c_str(), message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1)
		throw std::runtime_error("Failed to send message");

	return bytes_sent;
}

std::string Client::_create_line(const std::string &content) const
{
	std::string line = content;

	if (line.size() > _max_message_size - 2) {
		line.erase(_max_message_size - 7);
		line += "[CUT]";
	}
	line += "\r\n";

	return line;
}

std::string Client::_create_reply(reply_code code, const std::string &arg, const std::string &message) const
{
	std::ostringstream oss;
	oss << ':' << _server.get_name() << ' ' << std::setfill('0') << std::setw(3) << code << ' ' << get_nickname(false);

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty())
	{
		oss << ' ';
		if (message.find(' ') != std::string::npos)
			oss << ':';
		oss << message;
	}

	return _create_line(oss.str());
}

void Client::_check_registration()
{
	if (_nickname.empty() || _username.empty())
		return ;

	if (!_server.check_password(_password))
		return send_error("Access denied: Bad password?");

	_registered = true;

	_greet();
}

void Client::_greet() const
{
	std::string reply = // https://modern.ircdocs.horse/#rplwelcome-001
		_create_reply(RPL_WELCOME, "", "Welcome to the Internet Relay Network " + get_nickname() + '!' + _get_username() + '@' + _ip)
		+ _create_reply(RPL_YOURHOST, "", "Your host is " + _server.get_name() + ", running version " VERSION)
		+ _create_reply(RPL_CREATED, "", "This server has been started " + _server.get_datetime())
		+ _create_reply(RPL_MYINFO, _server.get_name() + " " VERSION " o iklt")
		+ _create_reply(RPL_ISUPPORT, "RFC2812 IRCD=ft_irc CHARSET=UTF-8 CASEMAPPING=ascii PREFIX=(o)@ CHANTYPES=#& CHANMODES=,k,l,it", "are supported on this server")
		+ _create_reply(RPL_ISUPPORT, "CHANLIMIT=#&:50 CHANNELLEN=50 NICKLEN=" + to_string(_max_nickname_size) + " TOPICLEN=490 AWAYLEN=127 KICKLEN=400 MODES=5", "are supported on this server")
		+ _create_reply(RPL_LUSERCLIENT, "", "There are " + to_string(_server.get_clients_count()) + " users and 0 services on 1 servers")
		+ _create_reply(RPL_LUSERCHANNELS, to_string(_server.get_channels_count()), "channels formed")
		+ _create_reply(RPL_LUSERME, "", "I have " + to_string(_server.get_clients_count()) + " users, 0 services and 0 servers");

	_send(reply);
}

const std::string Client::_get_username(bool truncate) const
{
	std::string username = "~";

	if (truncate)
		username += _username.substr(0, 18);
	else
		username += _username;

	return username;
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
