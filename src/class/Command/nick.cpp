#include "class/Client.hpp"
#include "class/Command.hpp"

void Command::_nick(const args_t &args, Client &client, Server &)
{
	client.set_nickname(args[1]);
}
