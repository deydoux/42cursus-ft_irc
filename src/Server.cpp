#include "Server.hpp"

#include <unistd.h>

#include <stdexcept>

void Server::_init() {
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw std::runtime_error("Failed to create socket");
	log("Socket created", debug);

	if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Failed to bind socket");
	log("Socket bound", debug);

	if (listen(_socket, 100) == -1)
		throw std::runtime_error("Failed to listen on socket");
	log("Socket listening", debug);

	_poll_fds.push_back((struct pollfd) {
		.fd = _socket,
		.events = POLLIN,
		.revents = 0
	});
}

void Server::_loop()
{
	log("Polling sockets", debug);
	if (poll(_poll_fds.data(), _poll_fds.size(), -1) == -1)
		throw std::runtime_error("Failed to poll sockets");
	log("Polled sockets", debug);

	_accept();
	_read();
}

void Server::_accept() {
	if (!(_poll_fds[0].revents & POLLIN))
		return;

	sockaddr_in address;
	socklen_t address_len = sizeof(address);

	int fd = accept(_socket, (sockaddr *)&address, &address_len);
	if (fd == -1)
		throw std::runtime_error("Failed to accept connection");
	log("Accepted connection");

	_poll_fds.push_back((struct pollfd) {
		.fd = fd,
		.events = POLLIN,
		.revents = 0
	});
}

void Server::_read() {
	for (_poll_fds_t::iterator it = _poll_fds.begin() + 1; it != _poll_fds.end(); ++it) {
		if (!(it->revents & POLLIN))
			continue;

		char buffer[1024];
		ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), MSG_DONTWAIT); // WAIT ✋ They don't love you like I love you

		if (bytes_read <= 0) {
			close(it->fd);
			_poll_fds.erase(it--);
			log("Closed connection");
			continue;
		}

		log("Received message:\n" + std::string(buffer, bytes_read));
	}
}

Server::Server(uint16_t port, std::string password, bool verbose):
	_verbose(verbose),
	_address((sockaddr_in) {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = (struct in_addr) {
			.s_addr = INADDR_ANY
		}
	}),
	_password(password)
{
	log("Constructed", debug);
}

Server::~Server() {
	for (_poll_fds_t::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
		close(it->fd);

	log("Destroyed", debug);
}

void Server::start() {
	_init();

	while (true)
		_loop();
}

void Server::log(const std::string &message, const log_level level) const {
	if (_verbose || level != debug)
		::log("Server", message, level);
}
