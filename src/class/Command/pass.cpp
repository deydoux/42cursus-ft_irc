#include "class/Client.hpp"
#include "class/Command.hpp"

static void pass_handler(const args_t &args, Client &client, Server &)
{
	client.set_password(args[1]);
}

const Command::_command_t Command::_pass = {
	.handler = &pass_handler,
	.min_args = 1,
	.max_args = 1,
	.register_mode = unregistered_only
};
