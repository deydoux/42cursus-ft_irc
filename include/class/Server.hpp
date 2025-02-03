#ifndef SERVER_HPP
#define SERVER_HPP

#include "lib.hpp"

#include <arpa/inet.h>
#include <poll.h>

class Client;

class Server
{
public:
	typedef uint16_t	port_t;

	Server(const std::string &name, port_t port, const std::string &password, bool verbose);
	~Server();

	void	log(const std::string &message, const log_level level = info) const;
	void	start();

	bool				check_password(const std::string &password) const;
	Client				*get_client(const std::string &nickname) const;
	const bool			&is_verbose() const;
	const std::string	&get_name() const;
	const std::string	&get_datetime() const;

	static bool	stop;

	static Server	parse_args(int argc, char *argv[]);

private:
	typedef std::vector<struct pollfd>	_pollfds_t;

	const std::string	_name;
	const port_t		_port;
	const std::string	_password;
	const bool			_verbose;
	const sockaddr_in	_address;

	int			_socket;
	_pollfds_t	_pollfds;
	std::string	_datetime;

	clients_t	_clients;
	channels_t	_channels;

	void	_set_datetime();
	void	_set_signal_handler();
	void	_init_socket();
	void	_bind();
	void	_listen();
	void	_init();
	void	_loop();
	void	_accept();
	void	_read();

	void	_disconnect_client(int fd);

	static const port_t			_default_port = 6697;
	static const bool			_default_verbose = true;

	static port_t			_parse_port(const std::string &port_str);
	static void				_print_usage(int status = 1);
	static sockaddr_in		_init_address(port_t port);
	static struct pollfd	_init_pollfd(int fd);
	static void				_signal_handler(int sig);
};

#endif // SERVER_HPP
