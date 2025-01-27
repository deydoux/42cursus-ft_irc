#include "class/Channel.hpp"

Channel::Channel(const std::string &name, const bool verbose):
	_name(name),
	_verbose(verbose)
{
	log("Created");
}

Channel::~Channel()
{
	log("Destroyed");
}

void Channel::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Channel " + _name, message, level);
}
