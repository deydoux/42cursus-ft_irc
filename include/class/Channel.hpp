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
	bool				is_full( void );
	bool				check_passkey(std::string &passkey);
	bool				is_invite_only( void );
	bool				is_client_banned(Client &client);
	bool				is_client_member(Client &client);

	void	send_broadcast(const std::string &message);

	void	set_passkey(std::string &passkey);
	void	set_max_members(size_t max_members);
	void	unset_members_limit( void );
	void	set_is_invite_only(bool invite_only);

	void	add_client(Client &client);

	static const size_t max_channel_name_size = 50;

	static bool	is_valid_name(const std::string &name);

private:
	const std::string	_name;
	clients_t			_members;
	Client				&_creator;

	std::string		_passkey;

	bool			_limit_members;
	size_t			_max_members;
	bool			_is_invite_only;
	std::vector<std::string> _banned_user_masks; // eg: nick!*@* , *!*@192.168.1.* ...

	const bool	_verbose;
};

#endif // CHANNEL_HPP
