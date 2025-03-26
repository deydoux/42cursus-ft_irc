#include "lib.hpp"

bool mask_compare(const std::string &mask, const std::string &str)
{
	size_t mask_pos = 0;

	size_t prev_pos;
	size_t wildcard_pos = std::string::npos;
	for (size_t pos = 0; pos < str.size();)
	{
		if (mask_pos < mask.size() && (mask[mask_pos] == str[pos] || mask[mask_pos] == '?')) {
			++mask_pos;
			++pos;
		}

		else if (mask_pos < mask.size() && mask[mask_pos] == '*') {
			wildcard_pos = mask_pos++;
			prev_pos = pos;
		}

		else if (wildcard_pos != std::string::npos) {
			mask_pos = wildcard_pos + 1;
			pos = ++prev_pos;
		}

		else
			return false;
	}

	while (mask_pos < mask.size() && mask[mask_pos] == '*')
		++mask_pos;

	return mask_pos == mask.size();
}
