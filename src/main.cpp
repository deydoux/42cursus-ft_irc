#include "class/Server.hpp"

int main(int argc, char *argv[])
{
	try {
		Server server = Server::parse_args(argc, argv);

		try {
			server.start();
		} catch (const std::exception &e) {
			server.log(e.what(), error);
			return 1;
		}

	} catch (int status) {
		return status;
	}

	return 0;
}
