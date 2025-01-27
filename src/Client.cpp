#include "Client.hpp"
#include "to_string.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>
#include <iostream>

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
	const std::string message = "Hello, world!\r\n";
	ssize_t bytes_sent = send(_fd, message.c_str(), message.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytes_sent == -1)
		throw std::runtime_error("Failed to send message");
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
	while ((pos = _buffer.find(CRLF)) != std::string::npos) {
		_handle_message(_buffer.substr(0, pos));
		_buffer.erase(0, pos + 2);
	}
}

void Client::_handle_message(std::string message)
{
	if (message.size() > 510) {
		// TODO: "ERROR :Closing connection: abc[~abc@z3r3p4.local] (Request too long)"
	}
}
