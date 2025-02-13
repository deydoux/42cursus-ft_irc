#include "class/Channel.hpp"
#include "class/Client.hpp"

Channel::Channel(Client &creator, const bool verbose = false): 
	_verbose(verbose),
	_creator(creator)
{
}

Channel::Channel(Client &creator, std::string &name, const bool verbose):
	_creator(creator),
	_verbose(verbose)
{
	log("Created");
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
	// - Must start with # or &
	if (!(name[0] == '&' || name[0] == '#'))
		return false;

	// - Max length check (50 chars)
	if (name.size() > 50)
		return false;
	
	// - No spaces, control chars, commas
	for (size_t i = 0; i < name.size(); i++) {
		if (iscntrl(name[i]) || name[i] == ' ')
			return false;
	}
	
	return true;
}

void Channel::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Channel " + _name, message, level);
}
