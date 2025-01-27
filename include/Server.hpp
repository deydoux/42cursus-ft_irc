#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "log.h"

#include <arpa/inet.h>
#include <poll.h>

#include <map>
#include <vector>

class Client;

class Server
{
public:
	typedef uint16_t	port_t;

	Server(port_t port, std::string password, bool verbose);
	~Server();

	void	log(const std::string &message, const log_level level = info) const;
	void	start();

	bool	is_verbose() const;

	static bool	stop;

	static Server	parse_args(int argc, char *argv[]);

private:
	typedef std::vector<struct pollfd>			_pollfds_t;
	typedef std::map<int, Client *>				_clients_t;
	typedef std::map<std::string, Channel *>	_channels_t;

	const port_t		_port;
	const sockaddr_in	_address;
	const std::string	_password;
	const bool			_verbose;

	int			_socket;
	_pollfds_t	_pollfds;

	_clients_t	_clients;
	_channels_t	_channels;

	void	_set_signal_handler();
	void	_init_socket();
	void	_bind();
	void	_listen();
	void	_init();
	void	_loop();
	void	_accept();
	void	_read();

	static port_t			_parse_port(const std::string &port_str);
	static void				_print_usage(int status = 1);
	static sockaddr_in		_init_address(port_t port);
	static struct pollfd	_init_pollfd(int fd);
	static void				_signal_handler(int sig);
};

#endif // SERVER_HPP
