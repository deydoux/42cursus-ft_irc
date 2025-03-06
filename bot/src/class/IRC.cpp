#include "class/IRC.hpp"

#include <sstream>
#include <vector>

const std::string IRC::_default_hostname = "127.0.0.1";

IRC::IRC(const std::string hostname, const port_t port, const std::string pass, bool verbose) :
	_hostname(hostname),
	_port(port),
	_password(pass),
	_verbose(verbose)
{
	this->log("Constructed", debug);
}

IRC::~IRC()
{
	this->log("Destroyed", debug);
}

void IRC::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("IRC Client", message, level);
}

void IRC::connect_to_irc_server( void )
{
	this->log("Trying to connect to " + _hostname + " from port " + to_string(_port) + " ...");
}

IRC IRC::launch_irc_client(int argc, char **argv)
{
	std::string password;
	port_t port = _default_port;
	std::string hostname = _default_hostname;
	bool verbose = _default_verbose;
	
	bool is_hostname_set = false;
	bool is_port_set = false;
	bool is_pass_set = false;

	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help")
			_print_usage(0);
		else if (arg == "-v" || arg == "--verbose")
			verbose = true;
		
		else if (arg == "-H" || arg == "--hostname") {
			hostname = _get_next_arg(argc, argv, i);
			is_hostname_set = true;
		} else if (arg == "-p" || arg == "--port") {
			port = _parse_port(_get_next_arg(argc, argv, i));
			is_port_set = true;
		} else if (arg == "-P" || arg == "--pass" || arg == "--password") {
			password = _get_next_arg(argc, argv, i);
			is_pass_set = true;
		} else if (arg[0] == '-') {
			_print_usage();
		}
		
		else if (!is_hostname_set) {
			hostname = arg;
			is_hostname_set = true;
		} else if (!is_port_set) {
			port = _parse_port(arg);
			is_port_set = true;
		} else if (!is_pass_set) {
			password = arg;
			is_pass_set = true;
		}

		else
			_print_usage();
	}

	IRC irc_client = IRC(hostname, port, password, verbose);

	if (!is_hostname_set)
		irc_client.log("Using default port: " + hostname, warning);
	if (!is_port_set)
		irc_client.log("Using default port: " + to_string(port), warning);
	if (!is_pass_set)
		irc_client.log("No password set", warning);
	
	irc_client.connect_to_irc_server();
	return irc_client;
}

std::string IRC::_get_next_arg(int argc, char *argv[], int &i)
{
	if (++i >= argc)
		_print_usage(0);

	return argv[i];
}

IRC::port_t IRC::_parse_port(const std::string &port_str)
{
	std::istringstream iss(port_str);
	port_t port;
	iss >> port;

	if (!iss.eof() || iss.fail())
		_print_usage();

	return port;
}

void IRC::_print_usage(int status)
{
	std::cerr << "Usage: ./ircbot [options]... [hostname] [port] [password]" << std::endl
			  << "  -h, --help                         Show this help message" << std::endl
			  << "  -H, --hostname <hostname>          Hostname (ip adress of the server host)" << std::endl
			  << "  -p, --port <port>                  Port to listen on (default: 6697)" << std::endl
			  << "  -P, --pass, --password <password>  Password required to connect (default: None)" << std::endl
			  << "  -v, --verbose                      Enable verbose output" << std::endl;

	throw status;
}
