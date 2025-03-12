#include "text_formatting.hpp"

std::string format(const std::string &str, const std::string &control_chars)
{
	return control_chars + str + control_chars;
}
