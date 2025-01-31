#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>

#include <sstream>

Client::Client(const int fd, char *ip, Server &server):
	_fd(fd),
	_ip(ip),
	_server(server),
	_registered(false)
{
	log("Accepted connection from " + std::string(_ip));
}

Client::~Client()
{
	close(_fd);
	log("Closed connection");
}

void Client::log(const std::string &message, const log_level level) const
{
	if (_server.is_verbose() || level != debug)
		::log("Client " + to_string(_fd), message, level);
}

void Client::init()
{
	_send("Hello, world!\r\n");
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
	while ((pos = _buffer.find('\n')) != std::string::npos) {
		_handle_message(_buffer.substr(0, pos));
		_buffer.erase(0, pos + 1);
	}
}

void Client::reply(reply_code code, const std::string &arg, const std::string &message) const
{
	std::ostringstream oss;
	oss << code << ' ' << get_nickname(false);

	if (!arg.empty())
		oss << ' ' << arg;

	if (!message.empty()) {
		oss << ' ';
		if (message.find(' ') != std::string::npos)
			oss << ':';
		oss << message;
	}

	_send(oss.str());
}

const bool &Client::is_registered() const
{
	return _registered;
}

// const std::string &Client::get_username() const
// {
// 	return _username;
// }

const std::string &Client::get_nickname(bool allow_empty) const
{
	static const std::string empty_nick = "*";

	if (!allow_empty && _nickname.empty())
		return empty_nick;

	return _nickname;
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
}

void Client::set_username(const std::string &username)
{
	_username = username;
}

void Client::set_password(const std::string &password)
{
	if (!_username.empty() && !_nickname.empty())
		return reply(ERR_ALREADYREGISTRED, "", "Connection already registered");

	_password = password;
}

ssize_t Client::_send(std::string message) const
{
	if (message.size() > _max_message_size - 2) {
		message.erase(_max_message_size - 7);
		message += "[CUT]";
	}

	log("Sending message: " + message, debug);
	message += "\r\n";

	ssize_t bytes_sent = send(_fd, message.c_str(), message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1)
		throw std::runtime_error("Failed to send message");

	return bytes_sent;
}

void Client::_handle_message(std::string message)
{
	if (message.empty())
		return;

	if (message.size() > _max_message_size - 1) {
		// TODO: "ERROR :Closing connection: abc[~abc@z3r3p4.local] (Request too long)"
		log("TODO: Request too long", error);
	}

	if (message[message.size() - 1] == '\r')
		message.erase(message.size() - 1);

	args_t args;
	size_t pos;
	while ((pos = message.find(' ')) != std::string::npos && message.find(':') != 0) {
		if (pos > 0)
			args.push_back(message.substr(0, pos));
		message.erase(0, pos + 1);
	}
	if (!message.empty()) {
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

	Command::execute(args, *this);
}

bool Client::_is_valid_nickname(const std::string &nickname)
{
	if (std::isdigit(nickname[0]) || nickname[0] == '-')
		return false;

	for (std::string::const_iterator it = nickname.begin(); it != nickname.end(); ++it) {
		if (!std::isalnum(*it) && std::string("-[]\\`_^{|}").find(*it) != std::string::npos)
			return false;
	}

	return true;
}
