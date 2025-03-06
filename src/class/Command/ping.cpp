#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	client.cmd_reply(server.get_name(), "PONG", server.get_name(), args[1]);
}

const Command::_command_t Command::_ping = {
	.handler = &handler,
	.min_args = 1,
	.max_args = 1,
	.need_registration = true
};
