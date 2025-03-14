#ifndef IRC_HPP
#define IRC_HPP

#include "lib.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstdio>
#include <iostream>
#include <map>

class TriviaGame;

class IRC
{
	public:
		typedef std::map<std::string, TriviaGame *> trivias_t; // channel_name: trivia ptr
		typedef uint16_t	port_t;

		bool	is_connected;

		IRC(const std::string hostname, const port_t port, const std::string pass, bool verbose);
		~IRC();

		void	log(const std::string &message, const log_level level = info) const;

		void	connect( void );
		void	send_registration( void );
		void	send_raw(const std::string &message, int send_delay = 0);
		std::string	create_reply(const std::string &cmd, std::string args = "", std::string message = "");
		std::string	receive( void );

		bool	is_playing(const std::string &channel_name);
		void	delete_trivia_game(TriviaGame *game);

		static bool	stop;

		static IRC	launch_irc_client(int argc, char **argv);

	private:
		const std::string	_hostname;
		const port_t		_port;
		const std::string	_password;
		const bool			_verbose;

		trivias_t			_ongoing_trivia_games;
		std::string			_inviting_client;
		std::vector<std::string> _trivia_request_sent;

		int					_socket_fd;

		void	_handle_messages(const std::string &messages);
		void	_handle_message(std::string message, bool last = false);
		void	_handle_command(const std::string &command, const std::vector<std::string> &args);
		void	_set_signal_handler( void );
		void	_update_games( void );
		void	_loop( void );

		static const std::string	_default_hostname;
		static const port_t			_default_port = 6697;
		static const bool			_default_verbose = true;

		static const std::string	_default_nickname;
		static const std::string	_default_username;
		static const std::string	_default_realname;

		static std::string	_get_next_arg(int argc, char *argv[], int &i);
		static port_t		_parse_port(const std::string &port_str);
		static void			_print_usage(int status = 1);
		static sockaddr_in	_init_address(port_t port);
		static void			_signal_handler(int sig);
};

#endif // IRC_HPP
