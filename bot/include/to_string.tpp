#ifndef TO_STRING_TPP
#define TO_STRING_TPP

#include <sstream>

template <typename T>
std::string to_string(T n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

#endif // TO_STRING_TPP
