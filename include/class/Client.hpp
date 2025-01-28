#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "lib.hpp"
#include "Server.hpp"

#include <unistd.h>

#include <string>

class Server;

class Client
{
public:
	Client(int fd, Server &server);
	~Client();

	void	log(const std::string &message, const log_level level = info) const;
	void	init();

	void	handle_messages(std::string messages);
private:
	const int	_fd;
	Server		&_server;

	std::string	_buffer;

	ssize_t _send(const std::string &message) const;
	void _handle_message(std::string message);

	static const size_t	_max_message_size = 512;
};

#endif // CLIENT_HPP
