#include "lib.hpp"

std::string get_ordinal(int n)
{
	if (n < 1)
		return "";

	int lastDigit = n % 10;
	int lastTwoDigits = n % 100;

	if (lastTwoDigits >= 11 && lastTwoDigits <= 13)
		return std::to_string(n) + "th";

	switch (lastDigit) {
		case 1: return std::to_string(n) + "st";
		case 2: return std::to_string(n) + "nd";
		case 3: return std::to_string(n) + "rd";
		default: return std::to_string(n) + "th";
	}
}
