#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <signal.h>
#include <stdio.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

Server::Server(const std::string &name, port_t port, const std::string &password, const std::string &motd, const std::string &motd_file, bool verbose):
	_name(name),
	_port(port),
	_password(password),
	_motd(motd),
	_motd_file(motd_file),
	_verbose(verbose),
	_address(_init_address(_port)),
	_connections(0),
	_max_connections(0),
	_max_registered_clients(0),
	_registered_clients_count(0)
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

bool Server::check_password(const std::string &password) const
{
	return password == _password;
}

void Server::register_client()
{
	_max_registered_clients = std::max(_max_registered_clients, ++_registered_clients_count);
}

const std::string &Server::get_name() const
{
	return _name;
}

const bool &Server::is_verbose() const
{
	return _verbose;
}

const std::string &Server::get_start_time() const
{
	return _start_time;
}

const std::vector<std::string> &Server::get_motd_lines() const
{
	return _motd_lines;
}

Client *Server::get_client(const std::string &nickname) const
{
	for (clients_t::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->get_nickname() == nickname)
			return it->second;
	}

	return NULL;
}

size_t Server::get_connections() const
{
	return _connections;
}

size_t Server::get_max_connections() const
{
	return _max_connections;
}

size_t Server::get_clients_count() const
{
	return _registered_clients_count;
}

size_t Server::get_max_clients() const
{
	return _max_registered_clients;
}

size_t Server::get_channels_count() const
{
	return _channels.size();
}

Server Server::parse_args(int argc, char *argv[])
{
	std::string name = "kittirc";
	Server::port_t port = _default_port;
	bool verbose = _default_verbose;
	std::string password;
	std::string motd;
	std::string motd_file = "kittirc.motd";

	bool port_set = false;
	bool password_set = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help")
			_print_usage(0);
		else if (arg == "-n" || arg == "--name")
			name = _get_next_arg(i, argc, argv);
		else if (arg == "-P" || arg == "--pass" || arg == "--password") {
			password = _get_next_arg(i, argc, argv);
			password_set = true;
		} else if (arg == "-p" || arg == "--port") {
			port = _parse_port(_get_next_arg(i, argc, argv));
			port_set = true;
		} else if (arg == "-m" || arg == "--motd")
			motd = _get_next_arg(i, argc, argv);
		else if (arg == "-M" || arg == "--motd-file")
			motd_file = _get_next_arg(i, argc, argv);
		else if (arg == "-v" || arg == "--verbose")
			verbose = true;
		else if (!port_set) {
			port = _parse_port(arg);
			port_set = true;
		} else if (!password_set) {
			password = arg;
			password_set = true;
		} else
			_print_usage();
	}

	Server server = Server(name, port, password, motd, motd_file, verbose);

	if (!port_set)
		server.log("Using default port: " + to_string(port), warning);

	if (!password_set)
		server.log("No password set", warning);

	return server;
}

bool Server::stop = false;

void Server::_init_motd()
{
	if (!_motd.empty())
		return _motd_lines.push_back(_motd);

	std::ifstream file(_motd_file.c_str());
	if (file.fail())
		return log("Failed to open MOTD file", warning);

	std::string line;
	while (std::getline(file, line))
		_motd_lines.push_back(line);

	file.close();
}

void Server::_set_start_time()
{
	time_t now = std::time(NULL);
	struct tm *tm = std::gmtime(&now);

	char datetime[64];
	std::strftime(datetime, sizeof(datetime), "%a %b %d %Y at %H:%M:%S (UTC)", tm);

	_start_time = datetime;
}

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
	_init_motd();
	_set_start_time();
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

	char *ip = inet_ntoa(address.sin_addr);
	if (!ip)
		return log("Failed to get client IP", error);

	_max_connections = std::max(_max_connections, ++_connections);

	_pollfds.push_back(_init_pollfd(fd));
	_clients[fd] = new Client(fd, ip, *this);
}

void Server::_read()
{
	for (_pollfds_t::iterator it = _pollfds.begin() + 1; it != _pollfds.end(); ++it) {
		if (!(it->revents & POLLIN))
			continue;

		char buffer[BUFSIZ];
		ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), MSG_DONTWAIT); // WAIT ✋ They don't love you like I love you

		if (bytes_read <= 0) {
			_disconnect_client((it--)->fd);
			continue;
		}

		bool disconnect_request;
		try {
			_clients[it->fd]->handle_messages(std::string(buffer, bytes_read));
			disconnect_request = _clients[it->fd]->has_disconnect_request();
		} catch (std::exception &e) {
			disconnect_request = true;
			_clients[it->fd]->log(e.what(), error);

			try {
				_clients[it->fd]->send_error("Internal server error");
			} catch (std::exception &e) {
				_clients[it->fd]->log(e.what(), error);
			}
		}

		if (disconnect_request)
			_disconnect_client((it--)->fd);
	}
}

void Server::_disconnect_client(int fd)
{
	_connections--;
	if (_clients[fd]->is_registered())
		_registered_clients_count--;

	for (channels_t::iterator it = _channels.begin(); it != _channels.end(); ++it)
		it->second->remove_client(*_clients[fd]);

	delete _clients[fd];
	_clients.erase(fd);

	for (_pollfds_t::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == fd) {
			_pollfds.erase(it);
			break;
		}
	}
}

std::string Server::_get_next_arg(int &i, int argc, char *argv[])
{
	if (++i >= argc)
		_print_usage();

	return argv[i];
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
			  << "  -n, --name <name>                  Name of the server (default: kittirc)" << std::endl
			  << "  -P, --pass, --password <password>  Password required to connect (default: None)" << std::endl
			  << "  -p, --port <port>                  Port to listen on (default: 6697)" << std::endl
			  << "  -m, --motd <message>               Message of the Day" << std::endl
			  << "  -M, --motd-file <file>             MOTD file (default: kittirc.motd)" << std::endl
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

Channel	*Server::find_channel(const std::string &channel_name)
{
	channels_t::iterator channel = _channels.find(channel_name);
	if (channel == _channels.end())
		return NULL;
	return channel->second;
}

void	Server::add_channel(Channel &new_channel)
{
	_channels[new_channel.get_name()] = &new_channel;
}
