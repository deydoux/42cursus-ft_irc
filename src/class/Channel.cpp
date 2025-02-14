#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "Channel.hpp"

void Channel::_default_initialization(Client &creator)
{
	_members[creator.get_fd()] = &creator;
	_members_limit = -1;
	_is_invite_only = false;
	_passkey = "";
}

Channel::Channel(Client &creator, const bool verbose = false): 
	_verbose(verbose), 
	_creator(creator)
{
	log("Created");
	_default_initialization(creator);
}

Channel::Channel(Client &creator, std::string &name, const bool verbose):
	_verbose(verbose),
	_creator(creator)
{
	log("Created");
	_default_initialization(creator);

	set_name(name);
}

Channel::~Channel()
{
	log("Destroyed");
}

void Channel::set_name(std::string &name, bool check_validity = true)
{
	if (check_validity && !is_valid_name(name))
		return ;

	_name = name;
}

const std::string	&Channel::get_name( void )
{
	return _name;
}

bool Channel::is_valid_name(const std::string &name)
{
	// - Max length check (50 chars)
	if (name.empty() || name.size() > _max_channel_name_size)
		return false;

	// - Must start with # or &
	if (!(name[0] == '&' || name[0] == '#'))
		return false;
	
	// - No spaces, control chars, commas
	for (size_t i = 0; i < name.size(); i++) {
		if (iscntrl(name[i]) || name[i] == ' ')
			return false;
	}
	
	return true;
}

void	Channel::set_members_limit(int members_limit)
{
	_members_limit = members_limit;
}

void Channel::set_is_invite_only(bool invite_only)
{
	_is_invite_only = invite_only;
}

void Channel::add_client(Client &client)
{
	_members[client.get_fd()] = &client;
}

bool Channel::is_full(void)
{
	return _members_limit == -1 || _members_limit <= _members.size();
}

void	Channel::set_passkey(std::string &passkey)
{
	_passkey = passkey;
}

bool	Channel::check_passkey(std::string &passkey)
{
	return _passkey == "" || _passkey == passkey;
}

bool Channel::is_invite_only(void)
{
	return _is_invite_only;
}

bool Channel::is_client_banned(Client &client)
{
	std::string	client_mask = client.get_mask();

	for (size_t i = 0; i < _banned_user_masks.size(); i++) {
		if (match_mask(_banned_user_masks[i], client_mask))
			return true;
	}
	return false;
}

bool Channel::is_client_member(Client &client)
{
	for (clients_t::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (*it->second == client)
			return true;
	}
	return false;
}

void	Channel::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Channel " + _name, message, level);
}
