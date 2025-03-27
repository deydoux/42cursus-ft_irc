#ifndef SERVER_HPP
#define SERVER_HPP

#include "lib.hpp"

#include <arpa/inet.h>
#include <poll.h>

class Client;

class Server
{
public:
// Types
	typedef uint16_t	port_t;

// Static attributes
	static bool	stop;

private:
// Types
	typedef std::vector<struct pollfd>	_pollfds_t;

// Static attributes
	static const bool	_default_verbose = false;
	static const port_t	_default_port = 6697;
	static const std::string	_default_motd_file;
	static const std::string	_default_name;

public:
// Static functions
	static Server	parse_args(int argc, char *argv[]);

// Constructors/destructors
	Server(const std::string &name, port_t port, const std::string &password, const std::string &motd, const std::string &motd_file, bool verbose);
	~Server();

// Methods
	void	log(const std::string &message, const log_level level = info) const;
	void	start();

// Getters
	// Server
	const std::string	&get_name() const;
	const std::string	&get_start_time() const;
	const std::vector<std::string>	&get_motd_lines() const;
	bool	check_password(const std::string &password) const;
	bool	is_verbose() const;
	// Server stats
	size_t	get_max_clients() const;
	size_t	get_max_registered_clients() const;
	size_t	get_registered_clients_count() const;
	// Channel
	const channels_t	&get_channels() const;
	const channels_t	get_channels(const std::string &mask) const;
	Channel	*get_channel(const std::string &channel_name) const;
	// Client
	const clients_t	&get_clients() const;
	const clients_t	get_clients(const std::string &mask) const;
	Client	*get_client(const std::string &nickname) const;

// Setters
	// Server stats
	void	register_client();
	// Channel
	void	add_channel(Channel &new_channel);
	void	delete_channel(const std::string &name);

private:
// Static functions
	static bool		_mask_compare(const std::string &mask, const std::string &str);
	static port_t	_parse_port(const std::string &port_str);
	static void		_print_usage(int status = 1);
	static void		_signal_handler(int sig);
	static sockaddr_in	_init_address(port_t port);
	static std::string	_get_next_arg(int &i, int argc, char *argv[]);
	static struct pollfd	_init_pollfd(int fd);

// Attributes
	const bool			_verbose;
	const port_t		_port;
	const sockaddr_in	_address;
	const std::string	_motd_file;
	const std::string	_motd;
	const std::string	_name;
	const std::string	_password;
	int	_socket;
	_pollfds_t	_pollfds;
	std::string	_start_time;
	std::vector<std::string>	_motd_lines;
	// Server stats
	size_t	_max_clients;
	size_t	_max_registered_clients;
	size_t	_registered_clients_count;
	// Channels
	channels_t	_channels;
	// Clients
	clients_t	_clients;

// Methods
	void	_accept();
	void	_bind();
	void	_down();
	void	_init_motd();
	void	_init_socket();
	void	_init();
	void	_listen();
	void	_loop();
	void	_receive();
	void	_set_signal_handler();
	void	_set_start_time();

// Setters
	// Clients
	void	_disconnect_client(int fd);
};

#endif // SERVER_HPP
