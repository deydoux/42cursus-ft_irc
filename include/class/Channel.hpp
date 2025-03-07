#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "lib.hpp"

class Channel
{
public:
	Channel(Client &creator, std::string &name, const bool verbose = false);
	~Channel();

	void	log(const std::string &message, const log_level level = info) const;

	const std::string	&get_name( void );
	const std::string	get_creation_timestamp( void ) const;
	const std::string	get_topic( void ) const;
	const std::string	get_topic_last_edited_at( void ) const;
	const std::string	get_topic_last_edited_by( void ) const;
	bool				is_full( void );
	bool				check_passkey(std::string &passkey);
	bool				is_invite_only( void );
	bool				is_client_banned(Client &client);
	bool				is_client_member(Client &client);
	bool				is_topic_protected( void );
	bool				is_client_invited(Client &client);

	void	send_broadcast(const std::string &message, int exclude_fd = -1);

	void	set_passkey(std::string &passkey);
	void	set_max_members(size_t max_members);
	void	unset_members_limit( void );
	void	set_is_invite_only(bool invite_only);
	void	set_is_topic_protected(bool is_topic_protected);
	void	set_topic(Client &editor, const std::string topic);

	void	invite_client(Client &client);
	void	add_client(Client &client);
	void	remove_client(Client &client);

	const clients_t	&get_members();
	const std::string list_members( void );

	static const size_t max_channel_name_size = 50;
	static const size_t max_topic_len = 490;

	static bool	is_prefix(const char &c);
	static bool	is_valid_name(const std::string &name);

	typedef struct modes_s {
		std::vector<std::string> flags; // eg: ["+k", "+l", "-i"]
		std::map<char, std::string> values; // eg: {'k': "pass", 'l': "10", ...}
	} modes_t;

	static std::string	stringify_modes(modes_t *modes, bool add_modes_values = true);
	std::string			get_modes(bool get_modes_values = true);
	void				add_modes(modes_t *modes);

private:
	const std::string	_name;
	clients_t			_members;
	const std::string	_creation_timestamp;
	clients_t			_invited_clients;

	std::string		_passkey;

	std::string		_topic;
	std::string		_topic_last_edited_at; // unix timestamp
	std::string		_topic_last_edited_by; // client nickname
	bool			_is_topic_protected;

	bool			_limit_members;
	size_t			_max_members;
	bool			_is_invite_only;

	std::vector<std::string>	_banned_user_masks; // eg: nick!*@* , *!*@192.168.1.* ...

	modes_t			_modes;

	const bool	_verbose;
};

#endif
