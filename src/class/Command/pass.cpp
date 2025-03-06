#include "class/Client.hpp"
#include "class/Command.hpp"

void Command::_pass(const args_t &args, Client &client, Server &)
{
	client.set_password(args[1]);
}
