#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "lib.hpp"

class Channel
{
public:
	Channel(Client &creator, const bool verbose = false);
	Channel(Client &creator, std::string &name, const bool verbose = false);
	~Channel();

	void	log(const std::string &message, const log_level level = info) const;

	const std::string	&get_name( void );
	bool				is_full( void );
	bool				check_passkey(std::string &passkey);
	bool				is_invite_only( void );

	void	set_name(std::string &name, bool check_validity = true);
	void	set_passkey(std::string &passkey);
	void	set_members_limit(int members_limit);
	void	set_is_invite_only(bool invite_only);

	static bool	is_valid_name(const std::string &name);

private:
	std::string		_name;
	clients_t		_members;
	Client			&_creator;

	std::string		_passkey;
	int				_members_limit;
	bool			_is_invite_only;

	const bool		_verbose;

	void	_default_initialization(Client &creator);
};

#endif // CHANNEL_HPP
