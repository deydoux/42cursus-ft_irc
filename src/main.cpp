#include "Server.hpp"

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	program_log("This is a debug message", debug);
	program_log("This is an info message", info);
	program_log("This is a warning message", warning);
	program_log("This is an error message", error);

	Server server(6697, "slay", true);
	server.log("This is a debug message through Server method", debug);
	server.log("This is an info message through Server method", info);
	server.log("This is a warning message through Server method", warning);
	server.log("This is an error message through Server method", error);
}
