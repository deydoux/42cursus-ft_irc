#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::_nick(const args_t &args, Client &client, Server &)
{
	client.set_nickname(args[1]);
}
