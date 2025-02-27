#include "lib.hpp"

std::string unix_timestamp( void )
{
	std::ostringstream oss;
	
	time_t now = time(NULL);
	oss << now;

	return oss.str();
}
