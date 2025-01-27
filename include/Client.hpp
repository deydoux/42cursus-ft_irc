#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "log.h"
#include "Server.hpp"

class Server;

class Client
{
public:
	Client(int fd, Server &server);
	~Client();

	void	log(const std::string &message, const log_level level = info) const;
	void	init();
	void	handle_message(const std::string &message);
private:
	const int	_fd;
	Server		&_server;
};

#endif // CLIENT_HPP
