#include "Server.hpp"

#include <unistd.h>

#include <sstream>
#include <stdexcept>

Server::Server(port_t port, std::string password, bool verbose):
	_port(port),
	_address(_init_address(_port)),
	_password(password),
	_verbose(verbose)
{
	log("Constructed", debug);
}

Server::~Server()
{
	for (pollfds_t::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
		close(it->fd);

	log("Destroyed", debug);
}

const sockaddr_in &Server::get_address() const
{
	return _address;
}

const std::string &Server::get_password() const
{
	return _password;
}

const bool &Server::is_verbose() const
{
	return _verbose;
}

void Server::start()
{
	_init();

	while (true)
		_loop();
}

void Server::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Server", message, level);
}

void Server::_init()
{
	std::ostringstream oss_port;
	oss_port << _port;

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw std::runtime_error("Failed to create socket");
	_pollfds.push_back(_init_pollfd(_socket));
	log("Socket created", debug);

	if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Failed to bind socket");
	log("Socket bound", debug);

	if (listen(_socket, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to listen on socket");
	log("Listening on port " + oss_port.str());
}

void Server::_loop()
{
	log("Polling sockets", debug);
	if (poll(_pollfds.data(), _pollfds.size(), -1) == -1)
		throw std::runtime_error("Failed to poll sockets");
	log("Polled sockets", debug);

	_accept();
	_read();
}

void Server::_accept()
{
	if (!(_pollfds[0].revents & POLLIN))
		return;

	sockaddr_in address;
	socklen_t address_len = sizeof(address);

	int fd = accept(_socket, (sockaddr *)&address, &address_len);
	if (fd == -1)
		throw std::runtime_error("Failed to accept connection");
	log("Accepted connection");

	_pollfds.push_back(_init_pollfd(fd));
}

void Server::_read()
{
	for (pollfds_t::iterator it = _pollfds.begin() + 1; it != _pollfds.end(); ++it) {
		if (!(it->revents & POLLIN))
			continue;

		char buffer[1024];
		ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), MSG_DONTWAIT); // WAIT ✋ They don't love you like I love you

		if (bytes_read <= 0) {
			close(it->fd);
			_pollfds.erase(it--);
			log("Closed connection");
			continue;
		}

		log("Received message:\n" + std::string(buffer, bytes_read));
	}
}

sockaddr_in Server::_init_address(port_t port)
{
	return (sockaddr_in) {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = (struct in_addr) {
			.s_addr = INADDR_ANY
		}
	};
}

pollfd Server::_init_pollfd(int fd)
{
	return (struct pollfd) {
		.fd = fd,
		.events = POLLIN,
		.revents = 0
	};
}
