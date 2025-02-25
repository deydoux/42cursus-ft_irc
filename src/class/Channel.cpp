#include "class/Channel.hpp"
#include "class/Client.hpp"

Channel::Channel(Client &creator, std::string &name, const bool verbose):
	_name(name),
	_passkey(),
	_limit_members(false),
	_is_invite_only(false),
	_verbose(verbose)
{
	log("Created", debug);
	_members[creator.get_fd()] = &creator;
}

Channel::~Channel()
{
	log("Destroyed", debug);
}

const std::string	&Channel::get_name( void )
{
	return _name;
}

bool Channel::is_valid_name(const std::string &name)
{
	// - Max length check (50 chars)
	if (name.empty() || name.size() > max_channel_name_size)
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

void	Channel::set_max_members(size_t max_members)
{
	_limit_members = true;
	_max_members = max_members;
}

void Channel::unset_members_limit(void)
{
	_limit_members = false;
}

void Channel::set_is_invite_only(bool invite_only)
{
	_is_invite_only = invite_only;
}

void Channel::invite_client(Client &client)
{
	_invited_clients[client.get_fd()] = &client;
}

void Channel::add_client(Client &client)
{
	_members[client.get_fd()] = &client;
}

void Channel::remove_client(int client_fd)
{
	_members.erase(client_fd);
	_invited_clients.erase(client_fd);
}

const clients_t &Channel::get_members()
{
	return _members;
}

bool Channel::is_full(void)
{
	return _limit_members && _max_members <= _members.size();
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
	return _members.find(client.get_fd()) != _members.end();
}

bool Channel::is_client_invited(Client &client)
{
	clients_t::iterator it = _invited_clients.find(client.get_fd());

	if (it == _invited_clients.end())
		return false;

	_invited_clients.erase(it);
	return true;
}

void Channel::send_broadcast(const std::string &message)
{
	for (clients_t::iterator member = _members.begin(); member != _members.end(); member++) {
		member->second->send(message);
	}
}

void Channel::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Channel " + _name, message, level);
}
