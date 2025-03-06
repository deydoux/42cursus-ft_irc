#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	if (args.size() == 2 && args[1] != server.get_name())
		return client.reply(ERR_NOSUCHSERVER, args[1], "No such server");

	const std::string &reply = client.create_motd_reply();
	client.send(reply);
}

const Command::_command_t Command::_motd = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 1,
	.need_registration = true
};
