#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "lib.hpp"

class Channel
{
public:
	Channel(Client &creator, const bool verbose = false);
	Channel(Client &creator, const std::string &name, const bool verbose = false);
	~Channel();

	void	log(const std::string &message, const log_level level = info) const;

	void	set_name(const std::string &name);

private:
	std::string		_name;
	const bool		_verbose;
	Client			&_creator;

	bool	_is_valid_name(const std::string &name);
};

#endif // CHANNEL_HPP
