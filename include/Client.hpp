#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "log.h"

class Client
{
public:
	Client(int fd, bool verbose = false);
	~Client();

	void	log(const std::string &message, const log_level level = info) const;
	void	init();
	void	handle_message(const std::string &message);
private:
	const int	_fd;
	const bool	_verbose;
};

#endif // CLIENT_HPP
