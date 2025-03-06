#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

// TODO ensure channel case insensitivity doesn't break
static void handler(const args_t &args, Client &client, Server &server)
{
	std::string mask = "*";
	if (args.size() > 1 && args[1] != "0")
		mask = args[1];

	bool operator_flag = args.size() > 2 && args[2] == "o";
	std::string context = "*";
	clients_t clients;

	if (Channel::is_prefix(mask[0])) {
		context = mask;
		Channel *channel = server.get_channel(mask);
		if (channel)
			clients = channel->get_members();
	} else if (!operator_flag) {
		clients = server.get_clients(mask);
	}

	std::string reply;
	for (clients_t::iterator it = clients.begin(); it != clients.end(); it++)
	{
		Client *found_client = it->second;
		reply += client.create_reply(
			RPL_WHOREPLY,
			found_client->generate_who_reply(context),
			"0 " + found_client->get_realname()
		);
	}

	reply += client.create_reply(RPL_ENDOFWHO, context, "End of WHO list");
	client.send(reply);
}

const Command::_command_t Command::_who = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 2,
	.register_mode = registred_only
};
