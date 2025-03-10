#include "class/IRC.hpp"

#include <sstream>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <cerrno>
#include <cstring>
#include <algorithm>

const std::string IRC::_default_hostname = "127.0.0.1";
const std::string IRC::_default_nickname = "hkitty";
const std::string IRC::_default_username = "hellokitty";
const std::string IRC::_default_realname = "Hello Kitty";

bool IRC::stop = false;

std::string ft_strerror( void )
{
	return std::string(std::strerror(errno));
}

std::string extract_nickname(const std::string& input) {
	size_t start = 0;
	size_t end = input.find('!');

	if (end == std::string::npos) return "";

	return input.substr(start, end - start);
}



IRC::IRC(const std::string hostname, const port_t port, const std::string pass, bool verbose) :
	is_connected(false),
	_hostname(hostname),
	_port(port),
	_password(pass),
	_verbose(verbose)
{
	this->log("Constructed", debug);
}

IRC::~IRC()
{
	send_command("QUIT", "", "Leaving");
	
	if (_socket_fd > 0)
		close(_socket_fd);
	this->log("Destroyed", debug);
}

void IRC::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("IRC Client", message, level);
}

void IRC::connect( void )
{
	this->log("Trying to connect to " + _hostname + " from port " + to_string(_port) + " ...");

	// Creating socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw std::runtime_error("Failed to create socket");

	this->log("Socket created", debug);

	// Set up server address struct
	struct sockaddr_in server_addr = _init_address(_port);

	if (inet_pton(AF_INET, _hostname.c_str(), &server_addr.sin_addr) <= 0)
		throw std::runtime_error("Invalid address");

	// Connect to server
	if (::connect(_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("Error connecting to server: " + ft_strerror());

	is_connected = true;
}

void IRC::send_raw(const std::string &message)
{
	if (!is_connected)
		throw std::runtime_error("Could not send message: client is not connected");

	ssize_t bytes_sent = send(_socket_fd, message.c_str(), message.length(), 0);
	if (bytes_sent < 0)
		throw std::runtime_error("Could not send message: " + ft_strerror());

	log("Sent message:\n" + message, debug);
}

void IRC::send_command(const std::string &cmd, std::string args, std::string message)
{
	if (!args.empty())
		args = " " + args;
	if (message.find(' ') != std::string::npos)
		message = ":" + message;
	if (!message.empty())
		message = " " + message;
	return this->send_raw(cmd + args + message + "\r\n");
}

void IRC::send_registration( void )
{
	if (!_password.empty())
		send_command("PASS", _password);
	send_command("NICK", _default_nickname);
	send_command("USER", _default_username + " 0 *", _default_realname);
}

std::string IRC::receive( void )
{
	if (!is_connected)
		throw std::runtime_error("Could not receive anything: client is not connected");

	char buffer[BUFSIZ];
	ssize_t bytes_received = recv(_socket_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received == 0)
		throw std::runtime_error("Connection closed by server");
	if (bytes_received < 0)
		throw std::runtime_error("Error receiving data: " + ft_strerror());

	return std::string(buffer, bytes_received);
}

void IRC::_signal_handler(int)
{
	stop = true;
}

void IRC::_loop( void )
{
	while (!stop)
	{
		std::string data = receive();
		if (data.empty())
			continue ;
		_handle_messages(data);
	}
}

void IRC::_handle_messages(const std::string &messages)
{
	size_t pos;
	std::string buffer = messages;
	while ((pos = buffer.find('\n')) != std::string::npos)
	{
		_handle_message(buffer.substr(0, pos));
		buffer.erase(0, pos + 1);
	}
}

void IRC::_handle_message(std::string message)
{
	if (message.empty())
		return;

	if (message[message.size() - 1] == '\r')
		message.erase(message.size() - 1);
	if (message[0] == ':')
		message.erase(0, 1);
	
	std::vector<std::string> args;
	args = ft_split(message.substr(0, message.find(':')), ' ');
	args.push_back(message.substr(message.find(':') + 1));

	std::ostringstream oss;
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
	{
		std::string arg = *it;
		if (arg[0] == ':')
			arg.erase(0);

		oss << '"' << arg << "\"";
		if (it + 1 != args.end())
			oss << ", ";
	}

	log("Parsed command: " + oss.str(), debug);

	if (args[1] == "INVITE")
	{
		if (args[2] != _default_nickname)
			return ;
		
		std::string channel = args[3];
		send_command("JOIN", channel);

		std::string greet_message = "Thanks for inviting me to this channel " + extract_nickname(args[0]) + "!";
		send_command("PRIVMSG", channel, greet_message);
	}
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
	
	irc_client._set_signal_handler();

	irc_client.connect();
	irc_client.send_registration();

	irc_client._loop();
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

void IRC::_set_signal_handler()
{
	struct sigaction act = {};
	act.sa_handler = _signal_handler;
	sigaction(SIGINT, &act, NULL);
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

sockaddr_in IRC::_init_address(port_t port)
{
	return (sockaddr_in) {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = 0,
		.sin_zero = {},
	};
}
