#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void names_handler(const args_t &args, Client &client, Server &server)
{
	std::string reply;

	if (args.size() == 1) {
		channels_t channels = server.get_channels();

		for (channels_t::iterator it = channels.begin(); it != channels.end(); ++it) {
			const std::string &channel_name = it->first;
			const Channel &channel = *it->second;

			reply += client.create_reply(RPL_NAMREPLY, "= " + channel_name, channel.get_names());
		}

		std::string lost_clients_nicknames;
		const clients_t &clients = server.get_clients();

		for (clients_t::const_iterator it = clients.begin(); it != clients.end(); ++it) {
			const Client &client = *it->second;

			if (client.get_channels().empty()) {
				if (!lost_clients_nicknames.empty())
					lost_clients_nicknames += " ";

				lost_clients_nicknames += client.get_nickname();
			}
		}

		if (!lost_clients_nicknames.empty())
			reply += client.create_reply(RPL_NAMREPLY, "* *", lost_clients_nicknames);
		reply += client.create_reply(RPL_ENDOFNAMES, "*", "End of NAMES list");
	} else {
		std::vector<std::string> channel_names = split(args[1], ',');
		for (size_t i = 0; i < channel_names.size(); ++i) {
			std::string &channel_name = channel_names[i];
			Channel *channel = server.get_channel(channel_name);

			if (channel) {
				channel_name = channel->get_name();
				reply += client.create_reply(RPL_NAMREPLY, "= " + channel_name, channel->get_names());
			}

			reply += client.create_reply(RPL_ENDOFNAMES, channel_name, "End of NAMES list");
		}
	}

	client.send(reply);
}

const Command::_command_t Command::_names = {
	.handler = &names_handler,
	.min_args = 0,
	.max_args = 1,
	.register_mode = registered_only
};
