#include "class/Client.hpp"
#include "class/Command.hpp"

void Command::_user(const args_t &args, Client &client, Server &)
{
	client.set_username(args[1]);
	// TODO check realname
	client.set_realname(args[4]);
}
