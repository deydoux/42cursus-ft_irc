#include "lib.hpp"

bool match_mask(const std::string &str, const std::string &mask) {
	size_t mask_pos = 0;
	size_t str_pos = 0;

	while (mask_pos < mask.length() && str_pos < str.length()) {
		if (mask[mask_pos] == '*') {
			while (mask_pos + 1 < mask.length() && mask[mask_pos + 1] == '*')
				++mask_pos;

			if (mask_pos == mask.length() - 1)
				return true;

			while (str_pos < str.length()) {
				if (match_mask(mask.substr(mask_pos + 1), str.substr(str_pos)))
					return true;
				++str_pos;
			}
			return false;
		}
		else if (mask[mask_pos] == '?' || mask[mask_pos] == str[str_pos]) {
			++mask_pos;
			++str_pos;
		}
		else {
			return false;
		}
	}

	while (mask_pos < mask.length() && mask[mask_pos] == '*')
		++mask_pos;

	return mask_pos == mask.length() && str_pos == str.length();
}
