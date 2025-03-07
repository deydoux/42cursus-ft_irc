#ifndef IRC_HPP
#define IRC_HPP

#include "lib.hpp"

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

class IRC
{
	public:
		typedef uint16_t	port_t;
		
		bool	is_connected;

		IRC(const std::string hostname, const port_t port, const std::string pass, bool verbose);
		~IRC();

		void	log(const std::string &message, const log_level level = info) const;
		
		void	connect( void );
		void	send_registration( void );
		void	send_command(const std::string &cmd, std::string args = "", std::string message = "");
		void	send_raw(const std::string &message);
		std::string	receive( void );

		static bool	stop;

		static IRC	launch_irc_client(int argc, char **argv);
		
	private:
		const std::string	_hostname;
		const port_t		_port;
		const std::string	_password;
		const bool			_verbose;

		int					_socket_fd;

		void	_handle_messages(const std::string &messages);
		void	_handle_message(std::string message);
		void	_set_signal_handler();
		void	_loop();

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
