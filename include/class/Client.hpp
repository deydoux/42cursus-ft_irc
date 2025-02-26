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

	void	join_channel(Channel &channel, std::string passkey);
	void	kick_channel(Channel &channel, std::string kicked_client, args_t args);
	void	notify_quit();

	void	reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void	cmd_reply(const std::string &prefix, const std::string &cmd, args_t &args) const;

	const std::string	create_motd_reply() const;

	const bool			&has_disconnect_request() const;
	const bool			&is_registered() const;
	bool				is_channel_operator(std::string channel_name) const;
	const std::string	&get_nickname(bool allow_empty = true) const;
	const int			&get_fd( void );
	std::string			get_mask( void ) const;
	channels_t			&get_active_channels( void );
	size_t				get_channels_count( void ) const;
	
	bool				is_channel_op(std::string channel_name) const;
	void				give_op_permissions_to(Channel &channel) const;
	void				remove_op_permissions_from(Channel &channel) const;
	Channel				*get_channel(const std::string &name);

	Server	&get_server() const;

	void	set_nickname(const std::string &nickname);
	void	set_username(const std::string &username);
	void	set_realname(const std::string &realname);
	void	set_password(const std::string &password);
	void	set_quit_reason(const std::string &reason);
	void	set_channel_operator(std::string channel);
	void	remove_channel_operator(std::string channel);

	bool	operator==(const Client &other) const;

	static const std::string	create_cmd_reply(const std::string &prefix, const std::string &cmd, args_t &args);

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

	std::string	_create_reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;

	void	_check_registration();
	void	_greet() const;

	const std::string	_get_username(bool truncate = true) const;

	static const size_t _max_channels = 50;
	static const size_t	_max_message_size = 512;
	static const size_t	_max_nickname_size = 9;

	static bool	_is_valid_nickname(const std::string &nickname);
	static bool	_is_valid_username(const std::string &username);

	static std::string	_create_line(const std::string &content);
};

#endif // CLIENT_HPP
