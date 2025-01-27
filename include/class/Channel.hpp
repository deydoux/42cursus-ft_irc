#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "lib.hpp"

#include <string>

class Channel
{
public:
	Channel(const std::string &name, const bool verbose = false);
	~Channel();

	void	log(const std::string &message, const log_level level = info) const;
private:
	const std::string	_name;
	const bool			_verbose;
};

#endif // CHANNEL_HPP
