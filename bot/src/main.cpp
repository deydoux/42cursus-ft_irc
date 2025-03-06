#include "class/IRC.hpp"

#include <iostream>

int main(int ac, char *av[])
{
	try {
		IRC kitty_bot = IRC::launch_irc_client(ac, av);
	} catch (int status) {
		return status;
	}

	return 0;
}
