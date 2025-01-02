#include "Server.hpp"

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	Server server(6697, "slay", true);
	server.start();
}
