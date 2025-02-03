#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>

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
	std::ostringstream oss;
	oss << ':' << _server.get_name() << ' ' <<  code << ' ' << get_nickname(false);

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty())
	{
		oss << ' ';
		if (message.find(' ') != std::string::npos)
			oss << ':';
		oss << message;
	}

	_send(oss.str());
}

void Client::send_error(const std::string &message)
{
	std::ostringstream oss;

	oss << "ERROR :Closing connection: " << get_nickname(false) << "[~" << _get_username() << '@' << _ip << ']';
	if (!message.empty())
		oss << " (" << message << ')';

	_send(oss.str());

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

ssize_t Client::_send(const std::string &message) const
{
	std::string send_message = message;

	if (send_message.size() > _max_message_size - 2)
	{
		send_message.erase(_max_message_size - 7);
		send_message += "[CUT]";
	}

	log("Sending message:\n" + send_message, debug);
	send_message += "\r\n";

	ssize_t bytes_sent = send(_fd, send_message.c_str(), send_message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1)
		throw std::runtime_error("Failed to send message");

	return bytes_sent;
}

void Client::_check_registration()
{
	if (_nickname.empty() || _username.empty())
		return ;

	if (!_server.check_password(_password))
		return send_error("Access denied: Bad password?");

	_registered = true;
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

const std::string Client::_get_username(bool truncate) const
{
	if (truncate)
		return _username.substr(0, 18);

	return _username;
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
