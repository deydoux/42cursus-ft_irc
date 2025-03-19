#include "lib.hpp"
#include <cstring>
#include <cerrno>

std::string ft_strerror( void )
{
	return std::string(std::strerror(errno));
}
