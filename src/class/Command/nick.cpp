#include "class/Client.hpp"
#include "class/Command.hpp"

static void handler(const args_t &args, Client &client, Server &)
{
	client.set_nickname(args[1]);
}

const Command::_command_t Command::_nick = {
	.handler = &handler,
	.min_args = 1,
	.max_args = 1,
	.need_registration = false
};
