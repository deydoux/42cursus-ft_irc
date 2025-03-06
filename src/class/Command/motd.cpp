#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::_motd(const args_t &args, Client &client, Server &server)
{
	if (args.size() == 2 && args[1] != server.get_name())
		return client.reply(ERR_NOSUCHSERVER, args[1], "No such server");

	const std::string &reply = client.create_motd_reply();
	client.send(reply);
}
