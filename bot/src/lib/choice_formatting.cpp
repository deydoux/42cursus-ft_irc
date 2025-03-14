#include "lib.hpp"

std::string format_choice(char letter, const std::string &choice)
{
	std::string result;
	std::ostringstream oss;

	oss << "[";
	oss << letter;
	oss << "]";

	result = format(oss.str(), BOLD);
	return result + " " + choice;
}

std::string format_inline_choices(const std::string &choice1, const std::string &choice2, int space_between)
{
	std::string result;

	result = choice1 + std::string(std::max(space_between, 20) - choice1.size(), ' ');
	return result + choice2;
}
