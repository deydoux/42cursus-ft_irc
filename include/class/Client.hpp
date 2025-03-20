#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "lib.hpp"

#include <unistd.h>

class Command;
class Server;

class Client
{
private:
// Static variables
	static const size_t	_max_kick_message_len = 400;
	static const size_t	_max_message_size = 512;
	static const size_t	_max_nickname_size = 9;
	static const size_t	_max_realname_len = 127;
	static const size_t _max_channels = 50;

public:
// Static functions
	static const std::string	create_cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "");

// Constructors/destructors
	Client(const int fd, const std::string ip, Server &server);
	~Client();

// Member functions
	const std::string	create_motd_reply() const;
	ssize_t				send(const std::string &message) const;
	std::string			create_reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	std::string			generate_who_reply(const std::string &context) const;
	void				broadcast(const std::string &message) const;
	void				cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "") const;
	void				handle_messages(std::string messages);
	void				log(const std::string &message, const log_level level = info) const;
	void				notify_quit() const;
	void				reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void				send_error(const std::string &message);

// Getters
	// Server
	Server	&get_server() const;
	// Channel
	Channel		*get_channel(const std::string &name) const;
	channels_t	&get_active_channels();
	size_t		get_channels_count() const;
	// Client
	bool				is_channel_operator(std::string channel_name) const;
	const bool			&has_disconnect_request() const;
	const bool			&is_registered() const;
	const int			&get_fd();
	const std::string	&get_nickname(bool allow_empty = true) const;
	std::string			get_mask() const;
	std::string			get_realname() const;

// Setters
	// Channel
	bool	join_channel(Channel &channel, std::string passkey);
	void	close_all_channels(std::string &reason);
	void	kick_channel(Channel &channel, const std::string &kicked_client, std::string &reason);
	void	part_channel(Channel &channel, std::string &reason);
	void	remove_channel_operator(std::string channel_name);
	void	set_channel_operator(std::string channel_name);
	void	set_quit_reason(const std::string &reason);
	// Client
	void	set_nickname(const std::string &nickname);
	void	set_password(const std::string &password);
	void	set_realname(const std::string &realname);
	void	set_username(const std::string &username);

	bool	operator==(const Client &other) const;

private:
// Static functions
	static bool			_is_valid_nickname(const std::string &nickname);
	static bool			_is_valid_username(const std::string &username);
	static std::string	_create_line(const std::string &content);

// Variables
	const int			_fd;
	const std::string	_ip;

	bool		_registered;
	std::string	_buffer;
	// Server
	bool		_disconnect_request;
	std::string	_quit_reason;
	Server		&_server;
	// Channel
	channels_t					_active_channels;
	std::vector<std::string>	_channel_operator;
	// Client
	std::string	_nickname;
	std::string	_password;
	std::string	_realname;
	std::string	_username;

// Member functions
	void	_check_registration();
	void	_greet() const;
	void	_handle_message(std::string message);

// Getters
	const std::string	_get_username(bool truncate = true) const;
};

#endif // CLIENT_HPP
