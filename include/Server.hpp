#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "log.h"

#include <arpa/inet.h>
#include <poll.h>

#include <map>
#include <vector>

class Server
{
public:
	typedef uint16_t	port_t;

	Server(port_t port = 6697, std::string password = "", bool verbose = false);
	~Server();

	void	log(const std::string &message, const log_level level = info) const;
	void	start();

private:
	typedef std::map<int, Client>		_clients_t;
	typedef std::vector<struct pollfd>	_pollfds_t;

	const port_t		_port;
	const sockaddr_in	_address;
	const std::string	_password;
	const bool			_verbose;

	int			_socket;
	_pollfds_t	_pollfds;

	_clients_t	_clients;

	void	_init();
	void	_loop();
	void	_accept();
	void	_read();

	static sockaddr_in		_init_address(port_t port);
	static struct pollfd	_init_pollfd(int fd);
};

#endif // SERVER_HPP
