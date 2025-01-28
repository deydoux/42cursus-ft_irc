#include "class/Client.hpp"

#include <sys/socket.h>
#include <sys/types.h>

#include <stdexcept>
#include <iostream>
#include <sstream>

Client::Client(const int fd, Server &server):
	_fd(fd),
	_server(server)
{
	log("Accepted connection");
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

ssize_t Client::_send(const std::string &message) const
{
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
	if (!message.empty())
		args.push_back(message);

	std::ostringstream oss;
	for (args_t::iterator it = args.begin(); it != args.end(); ++it) {
		oss << '"' << *it << "\"";
		if (it + 1 != args.end())
			oss << ", ";
	}
	log("Parsed command: " + oss.str(), debug);

	_server.execute_command(args, *this);
}
