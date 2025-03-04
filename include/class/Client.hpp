#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "lib.hpp"

#include <unistd.h>

class Command;
class Server;

class Client
{
public:
	Client(const int fd, const std::string ip, Server &server);
	~Client();

	void	handle_messages(std::string messages);
	void	log(const std::string &message, const log_level level = info) const;
	ssize_t	send(const std::string &message) const;
	void	send_error(const std::string &message);
	void	broadcast(const std::string &message) const;

	bool	join_channel(Channel &channel, std::string passkey);
	void	kick_channel(Channel &channel, const std::string &kicked_client, std::string &reason);
	void	part_channel(Channel &channel, std::string &reason);
	void	notify_quit();

	void	reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void	cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "") const;

	const std::string	create_motd_reply() const;

	const bool					&has_disconnect_request() const;
	const bool					&is_registered() const;
	bool						is_channel_operator(std::string channel_name) const;
	std::vector<std::string>	&get_channels_operator( void );
	const std::string			&get_nickname(bool allow_empty = true) const;
	const int					&get_fd( void );
	std::string					get_mask( void ) const;
	channels_t					&get_active_channels( void );
	size_t						get_channels_count( void ) const;
	std::string					get_realname( void ) const;
	std::string					generate_who_reply(const std::string &context) const;

	Channel				*get_channel(const std::string &name);

	Server	&get_server() const;

	void	set_nickname(const std::string &nickname);
	void	set_username(const std::string &username);
	void	set_realname(const std::string &realname);
	void	set_password(const std::string &password);
	void	set_quit_reason(const std::string &reason);
	void	set_channel_operator(std::string channel_name);
	void	remove_channel_operator(std::string channel_name);

	bool	operator==(const Client &other) const;

	static const std::string	create_cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "");
	std::string					create_reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;

private:
	const int			_fd;
	const std::string	_ip;
	Server				&_server;

	bool		_disconnect_request;
	bool		_registered;
	std::string	_buffer;

	std::string	_nickname;
	std::string	_password;
	std::string	_realname;
	std::string	_username;

	std::string	_quit_reason;

	channels_t						_active_channels;
	std::vector<std::string>		_channel_operator;

	void		_handle_message(std::string message);

	void	_check_registration();
	void	_greet() const;

	const std::string	_get_username(bool truncate = true) const;

	static const size_t _max_channels = 50;
	static const size_t	_max_message_size = 512;
	static const size_t	_max_nickname_size = 9;
	static const size_t	_max_kick_message_len = 400;

	static bool	_is_valid_nickname(const std::string &nickname);
	static bool	_is_valid_username(const std::string &username);

	static std::string	_create_line(const std::string &content);
};

#endif // CLIENT_HPP
