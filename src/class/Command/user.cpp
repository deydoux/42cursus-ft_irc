#include "class/Client.hpp"
#include "class/Command.hpp"

static void handler(const args_t &args, Client &client, Server &)
{
	client.set_username(args[1]);
	client.set_realname(args[4]);
}

const Command::_command_t Command::_user = {
	.handler = &handler,
	.min_args = 4,
	.max_args = 4,
	.register_mode = unregistered_only
};
