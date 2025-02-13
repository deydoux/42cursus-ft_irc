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

	void	set_name(std::string &name, bool check_validity = true);
	
	static bool	is_valid_name(const std::string &name);

private:
	std::string		_name;
	const bool		_verbose;
	Client			&_creator;
};

#endif // CHANNEL_HPP
