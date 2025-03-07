#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	std::string reply;

	if (args.size() == 1) {
		channels_t channels = server.get_channels();

		for (channels_t::iterator it = channels.begin(); it != channels.end(); it++)
			reply += client.create_reply(RPL_NAMREPLY, "= " + it->first, it->second->list_members());

		std::string lost_clients_nicknames;
		clients_t clients = server.get_clients("*");

		for (clients_t::iterator it = clients.begin(); it != clients.end(); it++) {
			if (it->second->get_channels_count() == 0) {
				if (!lost_clients_nicknames.empty()) lost_clients_nicknames += " ";
				lost_clients_nicknames += it->second->get_nickname();
			}
		}

		reply += client.create_reply(RPL_NAMREPLY, "* *", lost_clients_nicknames);
		reply += client.create_reply(RPL_ENDOFNAMES, "*", "End of NAMES list");
	} else {
		std::vector<std::string> channel_names = ft_split(args[1], ',');
		for (size_t i = 0; i < channel_names.size(); i++) {
			std::string &channel_name = channel_names[i];
			Channel *channel = server.get_channel(channel_name);

			if (channel) {
				channel_name = channel->get_name();
				reply += client.create_reply(RPL_NAMREPLY, "= " + channel_name, channel->list_members());
			}

			reply += client.create_reply(RPL_ENDOFNAMES, channel_name, "End of NAMES list");
		}
	}

	client.send(reply);
}

const Command::_command_t Command::_names = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 1,
	.register_mode = registered_only
};
