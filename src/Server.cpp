#include "Server.hpp"

void Server::log(const std::string &message, const log_level level) const {
	if (_verbose || level != debug)
		::log("Server", message, level);
}

Server::Server(uint16_t port, std::string password, bool verbose):
	_password(password),
	_verbose(verbose),
	_address((sockaddr_in) {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = {INADDR_ANY}
	})
{
	log("Constructed", debug);
}

Server::~Server() {
	log("Destroyed", debug);
}

void Server::start() {
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw std::runtime_error("Failed to create socket");
	log("Socket created", debug);

	if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Failed to bind socket");
	log("Socket bound", debug);
}
