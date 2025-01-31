#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "lib.hpp"

#include <unistd.h>

class Command;
class Server;

class Client
{
public:
	Client(int fd, char *ip, Server &server);
	~Client();

	void	log(const std::string &message, const log_level level = info) const;
	void	init();

	void	handle_messages(std::string messages);
	void	reply(int code, const std::string &arg, const std::string &message = "") const;

	const std::string	&get_username() const;
	const std::string	&get_nickname(bool allow_empty = true) const;

	void	set_password(const std::string &password);
private:
	const int	_fd;
	const char	*_ip;
	Server		&_server;

	std::string	_password;
	std::string	_username;
	std::string	_nickname;
	// std::string	_realname;

	std::string	_buffer;

	ssize_t	_send(std::string message) const;
	void	_handle_message(std::string message);

	static const size_t	_max_message_size = 512;
};

#endif // CLIENT_HPP
