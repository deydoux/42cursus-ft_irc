#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <signal.h>
#include <stdio.h>

#include <iostream>
#include <sstream>

Server::Server(port_t port, std::string password, bool verbose):
	_port(port),
	_address(_init_address(_port)),
	_password(password),
	_verbose(verbose)
{
	Command::init();
	log("Constructed", debug);
}

Server::~Server()
{
	close(_socket);
	for (clients_t::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
	for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
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

const std::string &Server::get_password() const
{
	return _password;
}

bool Server::is_verbose() const
{
	return _verbose;
}

void Server::disconnect_client(int fd)
{
	delete _clients[fd];
	_clients.erase(fd);
	for (_pollfds_t::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == fd) {
			_pollfds.erase(it);
			break;
		}
	}
}

Server Server::parse_args(int argc, char *argv[])
{
	Server::port_t port = _default_port;
	std::string password;
	bool verbose = _default_port;

	bool port_set = false;
	bool password_set = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help")
			_print_usage(0);
		else if (arg == "-P" || arg == "--pass" || arg == "--password") {
			if (++i >= argc)
				_print_usage();

			password = arg;
			password_set = true;
		} else if (arg == "-p" || arg == "--port") {
			if (++i >= argc)
				_print_usage();

			port = _parse_port(argv[i]);
			port_set = true;
		} else if (arg == "-v" || arg == "--verbose")
			verbose = true;
		else if (!port_set) {
			port = _parse_port(arg);
			port_set = true;
		} else if (!password_set) {
			password = arg;
			password_set = true;
		}
		else
			_print_usage();
	}

	Server server = Server(port, password, verbose);

	if (!port_set)
		server.log("Using default port: " + to_string(port), warning);

	if (!password_set)
		server.log("No password set", warning);

	return server;
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

	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Failed to set socket options");

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

	_pollfds.push_back(_init_pollfd(fd));
	char *ip = inet_ntoa(address.sin_addr);
	_clients[fd] = new Client(fd, ip, *this);
	try {
		_clients[fd]->init();
	} catch (std::runtime_error &e) {
		_clients[fd]->log(e.what(), error);
		disconnect_client(fd);
	}
}

void Server::_read()
{
	for (_pollfds_t::iterator it = _pollfds.begin() + 1; it != _pollfds.end(); ++it) {
		if (!(it->revents & POLLIN))
			continue;

		char buffer[BUFSIZ];
		ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), MSG_DONTWAIT); // WAIT ✋ They don't love you like I love you

		if (bytes_read <= 0) {
			disconnect_client((it--)->fd);
			continue;
		}

		_clients[it->fd]->handle_messages(std::string(buffer, bytes_read));
	}
}

Server::port_t Server::_parse_port(const std::string &port_str)
{
	std::istringstream iss(port_str);
	Server::port_t port;
	iss >> port;

	if (!iss.eof() || iss.fail())
		_print_usage();

	return port;
}

void Server::_print_usage(int status)
{
	std::cerr << "Usage: ./ircserv [options]... [port] [password]" << std::endl
			  << "  -h, --help                         Show this help message" << std::endl
			  << "  -P, --pass, --password <password>  Password required to connect (default: None)" << std::endl
			  << "  -p, --port <port>                  Port to listen on (default: 6697)" << std::endl
			  << "  -v, --verbose                      Enable verbose output" << std::endl;

	throw status;
}

sockaddr_in Server::_init_address(port_t port)
{
	return (sockaddr_in) {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = (struct in_addr) {
			.s_addr = INADDR_ANY
		},
		.sin_zero = {},
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
