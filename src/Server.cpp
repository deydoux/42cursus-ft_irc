#include "Server.hpp"

#include <signal.h>
#include <unistd.h>

#include <stdexcept>

std::string to_string(int n);

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
	for (_pollfds_t::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
		close(it->fd);
	log("Destroyed", debug);
}

void Server::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Server", message, level);
}

void Server::start()
{
	_init();
	while (!stop)
		_loop();
	log("Stopped");
}

bool Server::stop = false;

void Server::_set_signal_handler()
{
	struct sigaction act = {};
	act.sa_handler = _signal_handler;
	sigaction(SIGINT, &act, NULL);
}

void Server::_init_socket()
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		throw std::runtime_error("Failed to create socket");
	_pollfds.push_back(_init_pollfd(_socket));
	log("Socket created", debug);
}

void Server::_bind()
{
	if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Failed to bind socket");
	log("Socket bound", debug);
}

void Server::_listen()
{
	if (listen(_socket, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to listen on socket");
	log("Listening on port " + to_string(_port));
}

void Server::_init()
{
	_set_signal_handler();
	_init_socket();
	_bind();
	_listen();
}

void Server::_loop()
{
	log("Polling sockets", debug);
	if (poll(_pollfds.data(), _pollfds.size(), -1) == -1 && !stop)
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
		return log("Failed to accept connection", error);
	log("Accepted connection on fd " + to_string(fd));

	_pollfds.push_back(_init_pollfd(fd));
	_clients[fd] = Client();
}

void Server::_read()
{
	for (_pollfds_t::iterator it = _pollfds.begin() + 1; it != _pollfds.end(); ++it) {
		if (!(it->revents & POLLIN))
			continue;

		char buffer[1024];
		ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), MSG_DONTWAIT); // WAIT ✋ They don't love you like I love you

		if (bytes_read <= 0) {
			close(it->fd);
			_clients.erase(it->fd);
			log("Closed connection on fd " + to_string(it->fd));
			_pollfds.erase(it--);
			continue;
		}

		std::string message(buffer, bytes_read);
		for (size_t pos = 0; (pos = message.find('\t', pos)) != std::string::npos; pos += 2)
			message.replace(pos, 1, "\\t");
		for (size_t pos = 0; (pos = message.find('\n', pos)) != std::string::npos; pos += 2)
			message.replace(pos, 1, "\\n");
		for (size_t pos = 0; (pos = message.find('\r', pos)) != std::string::npos; pos += 2)
			message.replace(pos, 1, "\\r");

		log("Received message on fd " + to_string(it->fd) + '\n' + message);
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

void Server::_signal_handler(int)
{
	stop = true;
}
