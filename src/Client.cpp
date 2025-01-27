#include "Client.hpp"
#include "to_string.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
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

void Client::handle_message(const std::string &message)
{
	log("Received message:\n" + message);
}
