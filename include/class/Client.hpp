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
	static const size_t	_max_channels = 50;
	static const size_t	_max_message_size = 512;
	static const size_t	_max_nickname_size = 9;
	static const size_t	_max_realname_len = 127;
	static const size_t	_max_username_len = 18;

public:
// Static variables
	static const size_t	max_kick_reason_len = 400;

// Static functions
	static const std::string	create_cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "");
	static bool	is_valid_nickname(const std::string &nickname);


// Constructors/destructors
	Client(const int fd, const std::string ip, Server &server);
	~Client();

// Member functions
	// Client
	const std::string	create_motd_reply() const;
	const std::string	create_reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	const std::string	generate_who_reply(const std::string &context) const;
	ssize_t	send(const std::string &message) const;
	void	broadcast_quit() const;
	void	broadcast(const std::string &message) const;
	void	cmd_reply(const std::string &prefix, const std::string &cmd, const std::string &arg = "", const std::string &message = "") const;
	void	handle_messages(const std::string &messages);
	void	log(const std::string &message, const log_level level = info) const;
	void	reply(reply_code code, const std::string &arg = "", const std::string &message = "") const;
	void	send_error(const std::string &message);
	// Command
	void	join(const std::string &original_channel_name, Channel &channel, const std::string &passkey);
	void	kick(const std::string &nick_to_kick, Channel &channel, const std::string &reason);
	void	names(const std::string &channel_name);
	void	part(Channel &channel, const std::string &reason);

// Getters
	// Client
	const std::string	get_nickname(bool allow_empty = true) const;
	bool	is_registered() const;
	std::string	get_mask() const;
	std::string	get_realname() const;
	// Channel
	const channels_t	&get_channels() const;
	bool	is_channel_operator(std::string channel_name) const;
	Channel	*get_channel(const std::string &name) const;
	// Server
	bool	has_disconnect_request() const;
	int		get_fd() const;
	Server	&get_server() const;

// Setters
	// Client
	void	set_nickname(const std::string &nickname);
	void	set_realname(const std::string &realname);
	void	set_username(const std::string &username);
	// Channel
	void	delete_channel(const std::string &channel_name);
	void	remove_channel_operator(const std::string &channel_name);
	void	set_channel_operator(const std::string &channel_name);
	// Server
	void	set_password(const std::string &password);
	void	set_quit_reason(const std::string &reason);

private:
// Static functions
	static bool	_is_valid_username(const std::string &username);
	static std::string	_create_line(const std::string &content);

// Variables
	// Client
	bool	_registered;
	std::string	_buffer;
	std::string	_nickname;
	std::string	_realname;
	std::string	_username;
	// Channel
	channels_t	_channels;
	std::vector<std::string>	_channel_operator;
	// Server
	const int	_fd;
	const std::string	_ip;
	bool	_disconnect_request;
	Server	&_server;
	std::string	_password;
	std::string	_quit_reason;

// Member functions
	void	_check_registration();
	void	_greet() const;
	void	_handle_message(std::string message);

// Getters
	std::string	_get_username() const;
};

#endif // CLIENT_HPP
