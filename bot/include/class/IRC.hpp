#ifndef IRC_HPP
#define IRC_HPP

#include "class/Ollama.hpp"
#include "lib.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstdio>
#include <iostream>
#include <map>

#define RPL_NAMREPLY 353

class TriviaGame;

class IRC
{
	public:
		// -- TYPES
		typedef void (IRC::*command_handler_t)(const std::string, const std::vector<std::string> &);
		typedef struct command_s {
			command_handler_t	handler;
			size_t				nb_args;
		} command_t;
		typedef std::map<std::string, command_t> command_handlers_t;
		typedef std::map<std::string, TriviaGame *> trivias_t; // channel_name: trivia ptr
		typedef uint16_t	port_t;

		// -- PUBLIC ATTRIBUTES
		bool	is_connected;

		// -- CONSTRUCTOR + DESTRUCTOR
		IRC(const std::string hostname, const port_t port, const std::string pass, const std::string ollama_server_uri, const std::string ollama_model, bool verbose);
		~IRC();

		// -- PUBLIC METHODS
		std::string	create_reply(const std::string &cmd, std::string args = "", std::string message = "");
		void	send_raw(const std::string &message, int send_delay = 0);

		void	log(const std::string &message, const log_level level = info) const;

		// -- STATIC PUBLIC ATTRIBUTES
		static bool	stop;

		// -- STATIC METHODS
		static IRC			launch_irc_client(int argc, char **argv);
		static std::string	extract_nickname(const std::string &client_mask);

	private:
		// -- PRIVATE ATTRIBUTES
		bool				_last_command;
		command_handlers_t	_command_handlers;
		int					_socket_fd;
		std::string			_inviting_client;
		trivias_t			_ongoing_trivia_games;
		std::map<std::string, Ollama::context_t>	_ollama_contexts;
		std::vector<std::string>	_trivia_request_sent;
		Ollama				_ollama;

		// -- PRIVATE CONSTANTS
		const port_t		_server_port;
		const std::string	_server_hostname;
		const std::string	_server_password;
		const std::string	_nickname;
		const bool			_verbose;

		// -- PRIVATE METHODS
		void	_connect_to_server( void );
		void	_send_registration( void );
		void	_handle_message(std::string message);
		void	_handle_messages(const std::string &messages);
		std::string	_receive( void );
		void	_loop( void );
		void	_set_signal_handler( void );
		void	_update_games( void );
		bool	_is_playing(const std::string &channel_name);
		void	_handle_ollama(const std::string &origin, const std::string &nickname, const std::string &message);

		void	_handle_invite_command(const std::string sender_nickname, const std::vector<std::string> &args);
		void	_handle_join_command(const std::string sender_nickname, const std::vector<std::string> &args);
		void	_handle_kick_command(const std::string sender_nickname, const std::vector<std::string> &args);
		void	_handle_part_command(const std::string sender_nickname, const std::vector<std::string> &args);
		void	_handle_privmsg_command(const std::string sender_nickname, const std::vector<std::string> &args);
		void	_handle_numerics(int numeric, const std::vector<std::string> &args);
		void	_init_command_handlers( void );

		// -- PRIVATE STATIC CONSTANTS + ATTRIBUTES
		static const bool			_default_verbose = true;
		static const port_t			_default_port = 6697;
		static const std::string	_default_ollama_server_uri;
		static const std::string	_default_ollama_model;
		static const std::string	_default_hostname;
		static const std::string	_default_nickname;
		static const std::string	_default_realname;
		static const std::string	_default_username;


		// -- PRIVATE STATIC METHODS
		static std::string	_get_next_arg(int argc, char *argv[], int &i);
		static port_t		_parse_port(const std::string &port_str);
		static void			_print_usage(int status = 1);
		static sockaddr_in	_init_address(port_t port);
		static void			_signal_handler(int sig);
};

#endif // IRC_HPP
