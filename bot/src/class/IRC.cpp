#include "class/IRC.hpp"
#include "class/TriviaGame.hpp"

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
	send_raw(create_reply("QUIT", "", "Leaving"));

	for (trivias_t::iterator it = _ongoing_trivia_games.begin(); it != _ongoing_trivia_games.end(); it++) {
		delete it->second;
	}
	
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

void IRC::send_raw(const std::string &message, int send_after_ms)
{
	if (!is_connected)
		throw std::runtime_error("Could not send message: client is not connected");

	if (send_after_ms != 0)
		usleep(send_after_ms * 1000);

	ssize_t bytes_sent = send(_socket_fd, message.c_str(), message.length(), 0);
	if (bytes_sent < 0)
		throw std::runtime_error("Could not send message: " + ft_strerror());

	log("Sent message:\n" + message, debug);
}

std::string IRC::create_reply(const std::string &cmd, std::string args, std::string message)
{
	if (!args.empty())
		args = " " + args;
	if (message.find(' ') != std::string::npos)
		message = ":" + message;
	if (!message.empty())
		message = " " + message;
	return cmd + args + message + "\r\n";
}

void IRC::send_registration( void )
{
	std::string registration;
	
	if (!_password.empty())
		registration += create_reply("PASS", _password);
	registration += create_reply("NICK", _default_nickname);
	registration += create_reply("USER", _default_username + " 0 *", _default_realname);
	send_raw(registration);
}

std::string IRC::receive( void )
{
	if (!is_connected)
		throw std::runtime_error("Could not receive anything: client is not connected");

	char buffer[BUFSIZ];
	ssize_t bytes_received = recv(_socket_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	if (bytes_received == 0)
		throw std::runtime_error("Connection closed by server");
	if (bytes_received < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			_update_games();
		throw std::runtime_error("Error receiving data: " + ft_strerror());
	}

	return std::string(buffer, bytes_received);
}

void IRC::_signal_handler(int)
{
	stop = true;
}

void IRC::_update_games( void )
{
	for (trivias_t::iterator it = _ongoing_trivia_games.begin(); it != _ongoing_trivia_games.end(); it++)
		it->second->tick();
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
	_handle_command(args[1], args);
}

std::vector<std::string> IRC::get_clients_on_channel(const std::string &channel_name)
{
	(void)channel_name;
	// Needs to call NAME command, and parse the clients inside the channel
	std::vector<std::string> vec;
	return vec;
}

bool IRC::is_playing(const std::string &channel_name)
{
	return _ongoing_trivia_games.find(channel_name) != _ongoing_trivia_games.end();
}

void IRC::_handle_command(const std::string &command, const std::vector<std::string> &args)
{
	std::string sender_nickname = extract_nickname(args[0]);

	if (command == "INVITE") 
	{
		if (args[2] != _default_nickname)
			return ;
		
		std::string channel = args[3];
		send_raw(create_reply("JOIN", channel));
		_inviting_client = sender_nickname;
	}
	else if (command == "JOIN") 
	{
		std::string channel = args[2];
		
		if (_inviting_client.empty())
			return ;

		if (sender_nickname != _default_nickname && is_playing(channel)) {
			TriviaGame *game = _ongoing_trivia_games[channel];
			game->add_player(sender_nickname);
			return ;
		}

		std::string greet_message = "Thanks for inviting me to this channel " + _inviting_client + "!";
		send_raw(create_reply("PRIVMSG", channel, greet_message), 1000);

		_inviting_client = "";
	}
	else if (command == "PRIVMSG") 
	{
		if (std::string("&#").find(args[2][0]) == std::string::npos)
			// TODO: maybe act in a specific way if a pm is sent to HelloKitty ?
			return ;

		std::string channel = args[2];
		std::string message = args[3];
		bool playing = is_playing(channel);
		TriviaGame *game;

		if (!playing && message.substr(0, 7) == "~TRIVIA") {
			// TODO: Maybe put a limit of simultanious trivia game ?

			std::vector<std::string> clients_on_channel = get_clients_on_channel(channel);
			if (clients_on_channel.size() < 3)
				return send_raw(create_reply("PRIVMSG", channel, "Trivia needs at least two players to begin!"), 500);

			game = new TriviaGame(*this, channel, clients_on_channel);
			_ongoing_trivia_games[channel] = game;

			game->greet_players();
			return ;
		} else if (!playing) {
			return ;
		}

		game = _ongoing_trivia_games[channel];
		
		if (message.substr(0, 5) == "~STOP") {
			game->show_final_results();
			return ;
		}

		if (game->is_waiting_for_answers())
			game->store_answer(message, sender_nickname);
	}
	else if (command == "PART")
	{
		std::string channel = args[2];
		if (is_playing(channel))
		{
			TriviaGame *game = _ongoing_trivia_games[channel];
			game->remove_player(sender_nickname);
			return ;
		}
	}
}

void IRC::delete_trivia_game(TriviaGame *game)
{
	_ongoing_trivia_games.erase(game->get_channel());
	delete game;
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
		.sin_addr = {},
		.sin_zero = {},
	};
}
