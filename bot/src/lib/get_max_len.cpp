#include "lib.hpp"

int get_max_len(std::vector<std::string> arr)
{
	size_t max_len = 0;
	for (size_t i = 0; i < arr.size(); ++i) {
		if (arr[i].size() > max_len)
			max_len = arr[i].size();
	}

	return max_len;
}
