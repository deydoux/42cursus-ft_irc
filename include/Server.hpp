#ifndef SERVER_HPP
#define SERVER_HPP

#include "log.h"

#include <arpa/inet.h>
#include <poll.h>

#include <string>
#include <vector>

#define CRLF "\r\n"

class Server {
private:
	typedef std::vector<struct pollfd> _pollfds_t;

	const uint16_t _port;
	const sockaddr_in _address;
	const std::string _password;
	const bool _verbose;

	int _socket;
	_pollfds_t _pollfds;

	void _init();
	void _loop();
	void _accept();
	void _read();

	static sockaddr_in _init_address(uint16_t port);
	static struct pollfd _init_pollfd(int fd);
public:
	Server(uint16_t port, std::string password, bool verbose = false);
	~Server();
	void start();

	void log(const std::string &message, const log_level level = info) const;
};

#endif // SERVER_HPP
