#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <cstdlib>
#include <fstream>

static void handler(const args_t &args, Client &client, Server &server)
{
	std::string message = args.size() == 2 ? args[1] : "You've been Hello Kitty-ed !";
	std::ifstream aFile("hk.templates");

	if (aFile.fail())
		return server.log("Failed to open MOTD file", warning);

	int rand_display = std::rand() % 17;

	std::string line;
	while (std::getline(aFile, line))
	{
		std::string	rand_str = to_string(rand_display);
		if (line.find(rand_str) != std::string::npos)
		{
			while (std::getline(aFile, line) && line.find(to_string(rand_display + 1)) == std::string::npos)
				client.reply(RPL_HK, "", "- " + line);
			break ;
		}
	}
	aFile.close();
	client.reply(RPL_HK, "", message);
}

const Command::_command_t Command::_hk = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 1,
	.need_registration = false
};
