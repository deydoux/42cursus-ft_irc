#include "Server.hpp"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	Server server(6697, "slay", true);

	try {
		server.start();
	} catch (const std::exception &e) {
		server.log(e.what(), error);
		return 1;
	}

	return 0;
}
