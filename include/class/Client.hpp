#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "lib.hpp"

#include <unistd.h>

class Command;
class Server;

class Client
{
public:
	Client(const int fd, const std::string &ip, Server &server);
	~Client();

	void	handle_messages(std::string messages);
	void	log(const std::string &message, const log_level level = info) const;
	void	reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void	send_error(const std::string &message);
	void	invite_to_channel(Client &target, Channel &channel);

	const bool			&has_disconnect_request() const;
	const bool			&is_registered() const;
	const std::string	&get_nickname(bool allow_empty = true) const;
	const Server		&get_server( void ) const;
	const int			get_fd( void );
	const bool			is_invited_to(Channel &channel);
	const std::string	get_mask( void );

	void	set_nickname(const std::string &nickname);
	void	set_username(const std::string &username);
	void	set_realname(const std::string &realname);
	void	set_password(const std::string &password);

private:
	const int			_fd;
	const std::string	&_ip;
	Server				&_server;

	bool		_disconnect_request;
	bool		_registered;
	std::string	_buffer;

	std::string	_nickname;
	std::string	_password;
	std::string	_realname;
	std::string	_username;

	std::vector<std::string> _channel_invitations;

	void		_handle_message(std::string message);
	ssize_t		_send(const std::string &message) const;
	std::string _create_line(const std::string &content) const;
	std::string	_create_reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void		_check_registration();
	void		_greet() const;

	const std::string	_get_username(bool truncate = true) const;

	static const size_t	_max_message_size = 512;
	static const size_t	_max_nickname_size = 9;

	static bool	_is_valid_nickname(const std::string &nickname);
	static bool	_is_valid_username(const std::string &username);
};

#endif // CLIENT_HPP
