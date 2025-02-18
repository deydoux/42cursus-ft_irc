#include "lib.hpp"

bool match_mask(const std::string& mask, const std::string& user_string) {
	size_t mask_pos = 0;
	size_t str_pos = 0;
	
	while (mask_pos < mask.length() && str_pos < user_string.length()) {
		if (mask[mask_pos] == '*') {
			while (mask_pos + 1 < mask.length() && mask[mask_pos + 1] == '*')
				mask_pos++;
				
			if (mask_pos == mask.length() - 1)
				return true;
				
			while (str_pos < user_string.length()) {
				if (match_mask(mask.substr(mask_pos + 1), user_string.substr(str_pos)))
					return true;
				str_pos++;
			}
			return false;
		}
		else if (mask[mask_pos] == '?' || mask[mask_pos] == user_string[str_pos]) {
			mask_pos++;
			str_pos++;
		}
		else {
			return false;
		}
	}
	
	while (mask_pos < mask.length() && mask[mask_pos] == '*')
		mask_pos++;
		
	return mask_pos == mask.length() && str_pos == user_string.length();
}
