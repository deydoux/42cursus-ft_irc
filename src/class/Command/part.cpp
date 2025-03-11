#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void part_handler(const args_t &args, Client &client, Server &server)
{
	std::vector<std::string> channels_name = ft_split(args[1], ',');
	std::string reason = args.size() == 3 ? args[2] : "Leaving";

	for (size_t i = 0; i < channels_name.size(); i++)
	{
		std::string channel_name = channels_name[i];
		Channel *new_channel = server.get_channel(channel_name);
		if (!new_channel)
		{
			return client.reply(
				ERR_NOSUCHCHANNEL,
				channels_name[i],
				"No such channel"
			);
		}

		if (new_channel->is_client_member(client))
			return client.reply(ERR_NOTONCHANNEL, channel_name, "You're not on that channel");

		client.part_channel(*new_channel, reason);
	}
}

const Command::_command_t Command::_part = {
	.handler = &part_handler,
	.min_args = 1,
	.max_args = 2,
	.register_mode = registered_only
};
