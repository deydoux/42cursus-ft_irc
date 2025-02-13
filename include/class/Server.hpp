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

	Server(const std::string &name, port_t port, const std::string &password, const std::string &motd, const std::string &motd_file, bool verbose);
	~Server();

	void	log(const std::string &message, const log_level level = info) const;
	void	start();
	void	register_client();

	const std::string				&get_name() const;
	bool							check_password(const std::string &password) const;
	const std::vector<std::string>	&get_motd_lines() const;
	const bool						&is_verbose() const;
	const std::string				&get_start_time() const;

	Client							*get_client(const std::string &nickname) const;

	size_t							get_connections() const;
	size_t							get_max_connections() const;
	size_t							get_clients_count() const;
	size_t							get_max_clients() const;
	size_t							get_channels_count() const;

	static bool	stop;

	static Server	parse_args(int argc, char *argv[]);

private:
	typedef std::vector<struct pollfd>	_pollfds_t;

	const std::string	_name;
	const port_t		_port;
	const std::string	_password;
	const std::string	_motd;
	const std::string	_motd_file;
	const bool			_verbose;
	const sockaddr_in	_address;

	int			_socket;
	_pollfds_t	_pollfds;

	std::string					_start_time;
	std::vector<std::string>	_motd_lines;

	size_t	_connections;
	size_t	_max_connections;
	size_t	_max_registered_clients;
	size_t	_registered_clients_count;

	clients_t	_clients;
	channels_t	_channels;

	void	_init_motd();
	void	_set_start_time();
	void	_set_signal_handler();
	void	_init_socket();
	void	_bind();
	void	_listen();
	void	_init();
	void	_loop();
	void	_accept();
	void	_read();

	void	_disconnect_client(int fd);

	static const port_t	_default_port = 6697;
	static const bool	_default_verbose = true;

	static std::string		_get_next_arg(int &i, int argc, char *argv[]);
	static port_t			_parse_port(const std::string &port_str);
	static void				_print_usage(int status = 1);
	static sockaddr_in		_init_address(port_t port);
	static struct pollfd	_init_pollfd(int fd);
	static void				_signal_handler(int sig);
};

#endif // SERVER_HPP
