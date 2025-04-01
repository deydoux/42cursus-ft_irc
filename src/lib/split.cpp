#include "lib.hpp"

std::vector<std::string> split(const std::string &str, char separator)
{
	std::istringstream iss(str);
	std::vector<std::string> vec;

	std::string token;
	while (std::getline(iss, token, separator))
		vec.push_back(token);

	return vec;
}
