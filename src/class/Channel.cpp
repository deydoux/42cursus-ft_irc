#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Server.hpp"

#include <algorithm>

Channel::Channel(Client &creator, std::string &name, const bool verbose):
	_verbose(verbose),
	_creation_timestamp(to_string(time(NULL))),
	_name(name),
	_invite_only(false),
	_limit_members(false),
	_topic_protected(false),
	_passkey(),
	_topic()
{
	log("Created", debug);
	creator.set_channel_operator(name);
}

Channel::~Channel()
{
	log("Destroyed", debug);
}

const std::string	&Channel::get_name() const
{
	return _name;
}

bool Channel::is_prefix(const char &c)
{
	return (c == '#' || c == '&');
}

bool Channel::is_valid_name(const std::string &name)
{
	// - Max length check (50 chars)
	if (name.empty() || name.size() > max_channel_name_size)
		return false;

	// - Must start with # or &
	if (!is_prefix(name[0]))
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

void Channel::set_invite_only(bool status)
{
	_invite_only = status;
}

void Channel::invite_client(Client &client)
{
	_invited_clients[client.get_fd()] = &client;
}

void Channel::add_client(Client &client)
{
	_members[client.get_fd()] = &client;
}

void Channel::remove_client(Client &client)
{
	int client_fd = client.get_fd();

	_members.erase(client_fd);
	_invited_clients.erase(client_fd);

	client.remove_channel_operator(_name);
}

const clients_t &Channel::get_members() const
{
	return _members;
}

const std::string Channel::list_members()
{
	std::string result;

	for (clients_t::iterator it = _members.begin(); it != _members.end(); ++it) {
		Client &member = *it->second;
		bool is_op = member.is_channel_operator(_name);

		if (!result.empty())
			result += " ";

		result += std::string(is_op ? "@" : "") + member.get_nickname();
	}

	return result;
}

bool Channel::is_full() const
{
	return _limit_members && _max_members <= _members.size();
}

void	Channel::set_passkey(std::string &passkey)
{
	_passkey = passkey;
}

bool Channel::check_passkey(const std::string &passkey) const
{
	return passkey == _passkey;
}

bool Channel::is_invite_only() const
{
	return _invite_only;
}

bool Channel::is_client_banned(Client &client) const
{
	std::string	client_mask = client.get_mask();

	for (size_t i = 0; i < _banned_user_masks.size(); i++) {
		if (match_mask(_banned_user_masks[i], client_mask))
			return true;
	}

	return false;
}

bool Channel::is_client_member(Client &client) const
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

void Channel::broadcast(const std::string &message, int exclude_fd) const
{
	for (clients_t::const_iterator member = _members.begin(); member != _members.end(); member++)
		if (member->first != exclude_fd)
			member->second->send(message);
}

std::string Channel::get_modes(bool get_modes_values)
{
	return stringify_modes(_modes, get_modes_values);
}

const std::string Channel::get_creation_time() const
{
	return _creation_timestamp;
}

void Channel::set_topic_protection(bool status)
{
	_topic_protected = status;
}

bool Channel::is_topic_protected() const
{
	return _topic_protected;
}

void Channel::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Channel " + _name, message, level);
}

std::string Channel::stringify_modes(const modes_t &modes, bool values)
{
	if (modes.flags.empty())
		return "+";

	std::string str_flags;
	std::string str_values;
	char current_sign = 0;

	for (size_t i = 0; i < modes.flags.size(); ++i) {
		std::string mode = modes.flags[i];

		if (mode[0] != current_sign) {
			current_sign = mode[0];
			str_flags += current_sign;
		}

		str_flags += mode[1];
		if (modes.values.find(mode[1]) != modes.values.end())
			str_values += modes.values.find(mode[1])->second;
	}

	return str_flags + (values ? " " + str_values : "");
}

void Channel::add_modes(modes_t *modes)
{
	for (size_t i = 0; i < modes->flags.size(); i++)
	{
		char mode = modes->flags[i][1];
		if (mode == 'o') continue;

		bool is_adding = modes->flags[i][0] == '+';
		if (is_adding) {
			_modes.flags.push_back(modes->flags[i]);
		} else {
			_modes.flags.erase(std::find(
				_modes.flags.begin(),
				_modes.flags.end(),
				std::string("-") + mode
			));
		}
	}

	for (std::map<char, std::string>::iterator it = modes->values.begin(); it != modes->values.end(); ++it) {
		if (it->first == 'o') continue ;
		_modes.values[it->first] = it->second;
	}
}

const std::string Channel::get_topic( void ) const
{
	return _topic;
}

void Channel::set_topic(Client &author, const std::string topic)
{
	_topic = topic;
	_topic_author = author.get_nickname();
	_topic_edit_time = to_string(time(NULL));
}

const std::string Channel::get_topic_edit_time() const
{
	return _topic_edit_time;
}

const std::string Channel::get_topic_author() const
{
	return _topic_author;
}
