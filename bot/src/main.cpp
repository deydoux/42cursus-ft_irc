#include "class/IRC.hpp"

#include <iostream>

int main(int ac, char *av[])
{
	try {
		IRC kitty_bot = IRC::launch_irc_client(ac, av);
	} catch (std::exception &e) {
		log("IRC Client", e.what(), error);
		return 1;
	} catch (int error_status) {
		return error_status;
	}

	return 0;
}
