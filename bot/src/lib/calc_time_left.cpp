#include "lib.hpp"

int calc_time_left(std::time_t start, int total_seconds)
{
	time_t now = std::time(0);
	int time_passed = static_cast<int>(now - start);
	int time_remaining = total_seconds - time_passed;
	return time_remaining > 0 ? time_remaining : 0;
}
