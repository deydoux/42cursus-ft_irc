#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	if (args.size() == 1)
		return client.reply(ERR_NOORIGIN, "", "No origin specified");

	if (args.size() > 2 && args[2] != server.get_name())
		return client.reply(ERR_NOSUCHSERVER, args[2], "No such server");

	client.cmd_reply(server.get_name(), "PONG", server.get_name(), args[1]);
}

const Command::_command_t Command::_ping = {
	.handler = &handler,
	.min_args = 0,
	.max_args = -1,
	.register_mode = registered_only
};
