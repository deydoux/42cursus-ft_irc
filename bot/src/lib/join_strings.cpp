#include "lib.hpp"

std::string join_strings(const std::vector<std::string>& strings)
{
	if (strings.empty())
		return "";
	if (strings.size() == 1)
		return strings[0];

	std::ostringstream oss;
	for (size_t i = 0; i < strings.size() - 1; ++i) {
		if (i > 0)
			oss << ", ";
		oss << strings[i];
	}
	oss << " and " << strings.back();

	return oss.str();
}
