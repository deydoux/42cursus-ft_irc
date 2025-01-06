#include "Server.hpp"

#include <iostream>
#include <sstream>

static void help(int status = 1)
{
	std::cerr << "Usage: ./ircserv [options]... [port] [password]" << std::endl
		<< "  -h, --help                         Show this help message" << std::endl
		<< "  -P, --pass, --password <password>  Password required to connect (default: None)" << std::endl
		<< "  -p, --port <port>                  Port to listen on (default: 6697)" << std::endl
		<< "  -v, --verbose                      Enable verbose output" << std::endl;

	throw status;
}

static Server::port_t parse_port(const std::string &port_str)
{
	std::istringstream iss(port_str);
	Server::port_t port;
	iss >> port;

	if (!iss.eof() || iss.fail())
		help();

	return port;
}

static Server *init_server(int argc, char *argv[])
{
	Server::port_t port = 6697;
	std::string password;
	bool verbose = false;

	bool port_set = false;
	bool password_set = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help")
			help(0);
		else if (arg == "-P" || arg == "--pass" || arg == "--password") {
			if (++i >= argc)
				help();

			password = arg;
			password_set = true;
		} else if (arg == "-p" || arg == "--port") {
			if (++i >= argc)
				help();

			port = parse_port(argv[i]);
			port_set = true;
		} else if (arg == "-v" || arg == "--verbose")
			verbose = true;
		else if (!port_set) {
			port = parse_port(arg);
			port_set = true;
		} else if (!password_set) {
			password = arg;
			password_set = true;
		}
		else
			help();
	}

	Server *server = new Server(port, password, verbose);

	if (!port_set) {
		std::ostringstream oss_port;
		oss_port << port;
		server->log("Using default port: " + oss_port.str(), warning);
	}

	if (!password_set)
		server->log("No password set", warning);

	return server;
}

int main(int argc, char *argv[])
{
	Server *server;

	try {
		server = init_server(argc, argv);
	} catch (int status) {
		return status;
	}

	try {
		server->start();
	} catch (const std::exception &e) {
		server->log(e.what(), error);
		return 1;
	}

	delete server;

	return 0;
}
