#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &)
{
	const std::string &reason = args.size() == 2 ? args[1] : client.get_nickname();
	client.set_quit_reason(reason);

	const std::string &error = args.size() == 2 ? args[1] : "";
	client.send_error('"' + error + '"');
}

const Command::_command_t Command::_quit = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 1,
	.need_registration = true
};
