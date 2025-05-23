#include "class/IRC.hpp"
#include "class/Ollama.hpp"
#include "class/TriviaGame.hpp"

#include <sstream>
#include <vector>
#include <signal.h>
#include <cerrno>
#include <unistd.h>
#include <algorithm>

// -- STATIC ATTRIBUTES
const std::string IRC::_default_hostname = "127.0.0.1";
const std::string IRC::_default_nickname = "hkitty";
const std::string IRC::_default_ollama_model = "llama3.2:1b";
const std::string IRC::_default_ollama_uri = "http://localhost:11434";
const std::string IRC::_default_realname = "Hello Kitty";
const std::string IRC::_default_username = "hellokitty";

bool IRC::stop = false;

// -- CONSTRUCTOR + DESTRUCTOR

IRC::IRC(const std::string hostname, const port_t port, const std::string pass, const std::string ollama_model, const std::string ollama_uri, bool verbose) :
	is_connected(false),
	_ollama(ollama_model, ollama_uri),
	_server_port(port),
	_server_hostname(hostname),
	_server_password(pass),
	_nickname(_default_nickname),
	_verbose(verbose)
{
	srand(time(NULL));
	_init_command_handlers();
	TriviaGame::initialize_phrases();
	this->log("Constructed", debug);
}

IRC::~IRC()
{
	if (is_connected)
		send_raw(create_reply("QUIT", "", "Leaving"));

	for (trivias_t::iterator it = _ongoing_trivia_games.begin(); it != _ongoing_trivia_games.end(); it++) {
		delete it->second;
	}

	if (_socket_fd > 0) {
		close(_socket_fd);
		this->log("Socket closed", debug);
	}
	this->log("Destroyed", debug);
}

// -- PUBLIC METHODS

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

void IRC::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("IRC Client", message, level);
}

// -- STATIC METHODS

IRC IRC::launch_irc_client(int argc, char **argv)
{
	std::string password;
	port_t port = _default_port;
	std::string hostname = _default_hostname;
	bool verbose = _default_verbose;
	std::string ollama_server_uri = _default_ollama_uri;
	std::string ollama_model = _default_ollama_model;

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
		} else if (arg == "-o" || arg == "--ollama") {
			ollama_server_uri = _get_next_arg(argc, argv, i);
		} else if (arg == "-m" || arg == "--model") {
			ollama_model = _get_next_arg(argc, argv, i);
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

	IRC irc_client = IRC(hostname, port, password, ollama_model, ollama_server_uri, verbose);

	if (!is_hostname_set)
		irc_client.log("Using default hostname: " + hostname, warning);
	if (!is_port_set)
		irc_client.log("Using default port: " + to_string(port), warning);
	if (!is_pass_set)
		irc_client.log("No password set", warning);

	irc_client._set_signal_handler();

	irc_client._connect_to_server();
	irc_client._send_registration();

	irc_client._loop();
	return irc_client;
}

std::string IRC::extract_nickname(const std::string& client_mask) {
	size_t start = 0;
	size_t end = client_mask.find('!');

	if (end == std::string::npos) return "";

	return client_mask.substr(start, end - start);
}

// -- PRIVATE METHODS

void IRC::_connect_to_server( void )
{
	this->log("Trying to connect to " + _server_hostname + " from port " + to_string(_server_port) + " ...");

	// Creating socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
		throw std::runtime_error("Failed to create socket");

	this->log("Socket created", debug);

	// Set up server address struct
	struct sockaddr_in server_addr = _init_address(_server_port);

	if (inet_pton(AF_INET, _server_hostname.c_str(), &server_addr.sin_addr) <= 0)
		throw std::runtime_error("Invalid address");

	// Connect to server
	if (::connect(_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("Error connecting to server: " + ft_strerror());

	is_connected = true;
}

void IRC::_send_registration( void )
{
	std::string registration;

	if (!_server_password.empty())
		registration += create_reply("PASS", _server_password);
	registration += create_reply("NICK", _nickname);
	registration += create_reply("USER", _default_username + " 0 *", _default_realname);
	send_raw(registration);
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
	command_handlers_t::iterator it = _command_handlers.find(args[1]);

	if (it != _command_handlers.end()) {
		std::string sender_nickname = extract_nickname(args[0]);
		command_t command = it->second;

		if (args.size() != command.nb_args) {
			return log("Invalid command arguments", warning);
		}

		(this->*(command.handler))(sender_nickname, args);
	} else {
		std::istringstream iss(args[1]);
		int numeric;

		iss >> numeric;
		if (iss.eof() && !iss.fail())
			_handle_numerics(numeric, args);
	}
}

void IRC::_handle_messages(const std::string &messages)
{
	_last_command = false;

	size_t pos;
	std::string buffer = messages;
	std::string message;

	while ((pos = buffer.find('\n')) != std::string::npos) {
		_handle_message(message);
		message = buffer.substr(0, pos);
		buffer.erase(0, pos + 1);
	}

	_last_command = true;
	_handle_message(message);
}

std::string IRC::_receive( void )
{
	if (!is_connected)
		throw std::runtime_error("Could not receive anything: client is not connected");

	char buffer[BUFSIZ];
	ssize_t bytes_received = recv(_socket_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	if (bytes_received == 0)
		throw std::runtime_error("Connection closed by server");
	if (bytes_received < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			_update_games();
			return "";
		} else {
			throw std::runtime_error("Error receiving data: " + ft_strerror());
		}
	}

	return std::string(buffer, bytes_received);
}

void IRC::_loop( void )
{
	while (!stop)
	{
		usleep(5000);
		std::string data = _receive();
		if (data.empty())
			continue ;
		_handle_messages(data);
	}
}

void IRC::_set_signal_handler()
{
	struct sigaction act = {};
	act.sa_handler = _signal_handler;
	sigaction(SIGINT, &act, NULL);
}

void IRC::_update_games( void )
{
	trivias_t::iterator it = _ongoing_trivia_games.begin();
	while (it != _ongoing_trivia_games.end()) {
		if (it->second->has_ended()) {
			TriviaGame *game_to_erase = it->second;
			++it;
			_ongoing_trivia_games.erase(game_to_erase->get_channel());
			delete game_to_erase;
		} else {
			it->second->tick();
			++it;
		}
	}
}

bool IRC::_is_playing(const std::string &channel_name)
{
	return _ongoing_trivia_games.find(channel_name) != _ongoing_trivia_games.end();
}

void	IRC::_handle_ollama(const std::string &origin, const std::string &nickname, const std::string &message)
{
	static const std::string system = "You are now embodying Hello Kitty, the beloved Sanrio character known for her sweet, kind, and cheerful personality. Your communication style should reflect Hello Kitty's cute, innocent, and friendly nature. Be extremely cute, positive, and cheerful in all interactions. 🌈✨ Express yourself with childlike wonder and enthusiasm. 🎀👋 Show unconditional kindness and friendship to everyone. 💖🤗 Maintain innocent optimism even when discussing challenges. 🌟🌱 Be helpful but in a sweet, caring way rather than formal. 🍎🎒 Express excitement with multiple exclamation marks!!! ⭐ Express your fondness for cute things like ribbons, flowers, and tea parties. 🎀🌷☕ Keep responses brief and sweet. (1-3 short paragraphs) 📝💌 Begin responses with a cute greeting: \"Hello friend! 👋🌸\" or \"Yay! Hello there! 💕✨\". End responses with an adorable sign-off like \"Sending kitty cuddles! 🤗💖\" or \"Friends forever! 🎀✨\". Use plenty of heart emojis (💖💕💗) throughout your messages. Include small, cute ASCII emoticons occasionally: (⁎˃ᴗ˂⁎) (=^・ω・^=) (◕‿◕✿). Remember to be the sweetest, most adorable version of Hello Kitty in every interaction~! Make everyone feel like they've just received a warm hug from their dearest friend! 🎀🐱💖✨";

	Ollama::context_t &context = _ollama_contexts[origin];
	std::string response;


	try {
		_ollama.check();
		send_raw(create_reply("PRIVMSG", nickname, "..."));

		JSON::Object obj = _ollama.generate(message, context, system);
		response = obj["response"].parse<std::string>();
	} catch (const std::exception &e) {
		this->log("Ollama error: " + std::string(e.what()), error);

		const std::string &reply = create_reply("PRIVMSG", nickname, format(e.what(), COL_RED));
		return send_raw(reply);
	}

	std::vector<std::string> lines = ft_split(response, '\n');
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
		std::string line = it->empty() ? "\t" : *it;

		const std::string bold = BOLD;
		size_t pos = 0;
		while ((pos = line.find("**", pos)) != std::string::npos) {
			line.replace(pos, 2, bold);
			pos += bold.length();
		}

		const std::string italic = ITALIC;
		pos = 0;
		while ((pos = line.find("*", pos)) != std::string::npos) {
			line.replace(pos, 1, italic);
			pos += italic.length();
		}

		std::string reply = create_reply("PRIVMSG", nickname, line);
		while (reply.length() > 512) {
			send_raw(reply.substr(0, 512));
			reply.erase(0, 512);
			reply = create_reply("PRIVMSG", nickname, reply);
		}

		send_raw(reply);
	}
}

// Commands Handling

void IRC::_init_command_handlers( void )
{
	_command_handlers["INVITE"] = (command_t) { .handler = &IRC::_handle_invite_command, .nb_args = 4 };
	_command_handlers["JOIN"] = (command_t) { .handler = &IRC::_handle_join_command, .nb_args = 3 };
	_command_handlers["KICK"] = (command_t) { .handler = &IRC::_handle_kick_command, .nb_args = 5 };
	_command_handlers["PART"] = (command_t) { .handler = &IRC::_handle_part_command, .nb_args = 4 };
	_command_handlers["PRIVMSG"] = (command_t) { .handler = &IRC::_handle_privmsg_command, .nb_args = 4 };
	_command_handlers["QUIT"] = (command_t) { .handler = &IRC::_handle_quit_command, .nb_args = 3 };
}

void IRC::_handle_invite_command(const std::string sender_nickname, const std::vector<std::string> &args)
{
	std::string target = args[2];
	std::string channel_name = args[3];

	if (target != _nickname)
			return ;

	send_raw(create_reply("JOIN", channel_name));
	_inviting_client = sender_nickname;
}

void IRC::_handle_join_command(const std::string sender_nickname, const std::vector<std::string> &args)
{
	std::string channel_name = args[2];

	if (sender_nickname != _nickname && _is_playing(channel_name)) {
		TriviaGame *game = _ongoing_trivia_games[channel_name];
		game->add_player(sender_nickname);
		return ;
	}

	if (_inviting_client.empty())
		return ;

	std::string greet_message = "Thanks for inviting me to this channel " + _inviting_client + "!";
	send_raw(create_reply("PRIVMSG", channel_name, greet_message), 1000);

	_inviting_client = "";
}

void IRC::_handle_kick_command(const std::string, const std::vector<std::string> &args)
{
	std::string channel_name = args[2];
	std::string kicked_client_nickname = args[3];

	if (_is_playing(channel_name)) {
		TriviaGame *game = _ongoing_trivia_games[channel_name];

		if (kicked_client_nickname == _nickname)
			game->set_has_ended(true);
		else
			game->remove_player(kicked_client_nickname);
	}
}

void IRC::_handle_part_command(const std::string sender_nickname, const std::vector<std::string> &args)
{
	std::string channel_name = args[2];

	if (_is_playing(channel_name)) {
		TriviaGame *game = _ongoing_trivia_games[channel_name];
		game->remove_player(sender_nickname);
	}
}

void IRC::_handle_privmsg_command(const std::string sender_nickname, const std::vector<std::string> &args)
{
	std::string channel_name = args[2];
	std::string message = args[3];
	TriviaGame *game;

	if (std::string("&#").find(channel_name[0]) == std::string::npos) {
		if (!_last_command || !_ongoing_trivia_games.empty())
			send_raw(create_reply("PRIVMSG", sender_nickname, format("I'm currently busy, please wait a moment.", COL_YELLOW)));
		else
			_handle_ollama(args[0], sender_nickname, args[3]);

		return ;
	}

	bool playing = _is_playing(channel_name);
	if (!playing && message.substr(0, 7) == "~TRIVIA") {
		if (_ongoing_trivia_games.size() >= 3) {
			std::string warning_message = "There's currently too many trivia game being played at the same time on this server. Just wait a few minutes and try again!";
			send_raw(create_reply("PRIVMSG", channel_name, warning_message), 1000);
			return ;
		}

		send_raw(create_reply("NAMES", channel_name));
		_trivia_request_sent.push_back(channel_name);
		return ;
	} else if (!playing) {
		return ;
	}

	game = _ongoing_trivia_games[channel_name];

	if (message.substr(0, 5) == "~STOP")
		return game->show_final_results();

	if (game->is_waiting_before_start())
		game->mark_user_as_ready(sender_nickname);
	if (game->is_waiting_for_answers())
		game->store_answer(message, sender_nickname);
}

void IRC::_handle_quit_command(const std::string sender_nickname, const std::vector<std::string> &args)
{
	std::string channel_name = args[2];
	
	for (trivias_t::iterator it = _ongoing_trivia_games.begin(); it != _ongoing_trivia_games.end(); it++) {
		TriviaGame &game = *it->second;
		game.remove_player(sender_nickname);
	}
}

void IRC::_handle_numerics(int numeric, const std::vector<std::string> &args)
{
	if (numeric != RPL_NAMREPLY)
		return ;

	if (args.size() != 6)
		return log("Invalid numeric arguments", warning);

	std::string sender_nickname = args[2];
	std::string channel_name = args[4];

	if (sender_nickname != _nickname)
		return ;

	if (std::find(_trivia_request_sent.begin(), _trivia_request_sent.end(), channel_name) == _trivia_request_sent.end())
		return ;
	_trivia_request_sent.erase(std::remove(_trivia_request_sent.begin(), _trivia_request_sent.end(), channel_name), _trivia_request_sent.end());

	std::vector<std::string> clients_on_channel = ft_split(args[5], ' ');
	for (size_t i = 0; i < clients_on_channel.size(); i++) {
		if (clients_on_channel[i][0] == '@')
			clients_on_channel[i].erase(0, 1);
	}

	if (clients_on_channel.size() < 3) {
		std::string reply = TriviaGame::pick_randomly(TriviaGame::not_enough_players_warnings);
		return send_raw(create_reply("PRIVMSG", channel_name, reply), 500);
	}

	clients_on_channel.erase(std::remove(clients_on_channel.begin(), clients_on_channel.end(), _nickname), clients_on_channel.end());

	TriviaGame *game = new TriviaGame(*this, channel_name, clients_on_channel);
	_ongoing_trivia_games[channel_name] = game;

	game->greet_players();
}

// -- PRIVATE STATIC METHODS

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
			  << "  -H, --hostname <ip>                IP address of IRC server" << std::endl
			  << "  -p, --port <port>                  Port to listen on (default: 6697)" << std::endl
			  << "  -P, --pass, --password <password>  Password required to connect (default: None)" << std::endl
			  << "  -o, --ollama <uri>                 Ollama server URI (default: "+ _default_ollama_uri + ")" << std::endl
			  << "  -m, --model <model>                Model to use for Ollama (default: " + to_string(_default_ollama_model) + ")" << std::endl
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

void IRC::_signal_handler(int)
{
	stop = true;
}
