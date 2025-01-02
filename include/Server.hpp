#ifndef SERVER_HPP
#define SERVER_HPP

#include "log.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdexcept>
#include <string>

#define CRLF "\r\n"

class Server {
private:
	const std::string _password;
	const bool _verbose;

	const sockaddr_in _address;

	int _socket;

public:
	Server(uint16_t port, std::string password, bool verbose = false);
	~Server();
	void start();

	void log(const std::string &message, const log_level level = info) const;
};

#endif // SERVER_HPP
