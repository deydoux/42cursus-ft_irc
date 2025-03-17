#include "text_formatting.hpp"

std::string format(const std::string &str, const std::string &control_chars)
{
	std::string end_control_chars = control_chars;
	if (control_chars[0] == '\x03')
		end_control_chars = COL_BLACK;

	return control_chars + str + end_control_chars;
}
