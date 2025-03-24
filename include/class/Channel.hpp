#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "lib.hpp"

class Channel
{
public:
// Types
	typedef struct modes_s {
		std::vector<std::string>	flags; // e.g. ["+k", "+l", "-i"]
		std::map<char, std::string>	values; // e.g. {'k': "pass", 'l': "10"}
	} modes_t;

// Static attributes
	static const size_t max_channel_name_size = 50;
	static const size_t max_topic_len = 490;

// Static functions
	static bool			is_prefix(const char &c);
	static bool			is_valid_name(const std::string &name);
	static std::string	stringify_modes(const modes_t &modes, bool add_modes_values = true);

// Constructors/destructors
	Channel(Client &creator, std::string &name, const bool verbose = false);
	~Channel();

// Methods
	void	log(const std::string &message, const log_level level = info) const;
	void	broadcast(const std::string &message, int exclude_fd = -1) const;

// Getters
	// Channel
	const std::string	&get_name() const;
	const std::string	get_creation_time() const;
	const std::string	get_topic_author() const;
	const std::string	get_topic_edit_time() const;
	const std::string	get_topic() const;
	bool	check_passkey(const std::string &passkey) const;
	bool	is_full() const;
	bool	is_invite_only() const;
	bool	is_topic_protected() const;
	// Client
	const clients_t	&get_members() const;
	bool	is_client_banned(Client &client) const;
	bool	is_client_invited(Client &client);
	bool	is_client_member(Client &client) const;

// Setters
	// Channel
	void	set_passkey(std::string &passkey);
	void	set_max_members(size_t max_members);
	void	unset_members_limit();
	void	set_invite_only(bool status);
	void	set_topic_protection(bool status);
	void	set_topic(Client &author, const std::string topic);
	// Client
	void	invite_client(Client &client);
	void	add_client(Client &client);
	void	remove_client(Client &client);


	const std::string list_members();

	std::string	get_modes(bool get_modes_values = true);
	void				add_modes(modes_t *modes);

private:
// Attributes
	const bool			_verbose;
	const std::string	_creation_timestamp;
	const std::string	_name;
	bool		_invite_only;
	bool		_limit_members;
	bool		_topic_protected;
	clients_t	_invited_clients;
	clients_t	_members;
	modes_t		_modes;
	size_t		_max_members;
	std::string	_passkey;
	std::string	_topic_author;
	std::string	_topic_edit_time;
	std::string	_topic;
	std::vector<std::string>	_banned_user_masks; // e.g. ["nick!*@*", "*!*@192.168.1.*"]
};

#endif
