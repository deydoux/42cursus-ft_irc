#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	std::string mask = args.size() > 1 && args[1] != "0" ? args[1] : "*";
	const bool operator_flag = args.size() > 2 && args[2] == "o";

	clients_t clients;

	if (Channel::is_prefix(mask[0])) {
		Channel *channel = server.get_channel(mask);

		if (channel)
			clients = channel->get_members();

		mask = channel->get_name();
	} else {
		clients = server.get_clients(mask);
	}

	std::string reply;
	if (!operator_flag) {
		for (clients_t::iterator it = clients.begin(); it != clients.end(); ++it) {
			Client *found_client = it->second;

			reply += client.create_reply(RPL_WHOREPLY, found_client->generate_who_reply(mask), "0 " + found_client->get_realname());
		}
	}

	reply += client.create_reply(RPL_ENDOFWHO, mask, "End of WHO list");
	client.send(reply);
}

const Command::_command_t Command::_who = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 2,
	.register_mode = registered_only
};
