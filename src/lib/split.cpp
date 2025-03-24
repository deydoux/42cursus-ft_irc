#include "lib.hpp"

std::vector<std::string> split(const std::string &str, char separator)
{
	std::istringstream ss(str);
	std::vector<std::string> result;
	std::string token;

	while (std::getline(ss, token, separator)) {
		result.push_back(token);
	}
	return result;
}
