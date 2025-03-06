#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::_quit(const args_t &args, Client &client, Server &)
{
	const std::string &reason = args.size() == 2 ? args[1] : client.get_nickname();
	client.set_quit_reason(reason);

	const std::string &error = args.size() == 2 ? args[1] : "";
	client.send_error('"' + error + '"');
}
