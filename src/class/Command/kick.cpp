#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void kick_handler(const args_t &args, Client &client, Server &server)
{
	std::vector<std::string> channels_name = ft_split(args[1], ',');
	std::vector<std::string> kicked_client = ft_split(args[2], ',');
	std::string reason = args.size() == 4 ? args[3] : client.get_nickname();

	std::vector<Channel *> channels_to_kick_from;

	for (size_t i = 0; i < channels_name.size(); i++) {
		std::string channel_name = channels_name[i];
		Channel *new_channel = server.get_channel(channel_name);

		if (!new_channel)
			client.reply(ERR_NOSUCHCHANNEL, channel_name, "No such channel");
		else
			channels_to_kick_from.push_back(new_channel);
	}

	if (reason.size() > Client::max_kick_reason_len)
		reason.resize(Client::max_kick_reason_len);

	for (size_t i = 0; i < kicked_client.size(); i++)
		for (size_t j = 0; j < channels_to_kick_from.size(); j++)
			client.kick(*channels_to_kick_from[j], kicked_client[i], reason);
}

const Command::_command_t Command::_kick = {
	.handler = &kick_handler,
	.min_args = 2,
	.max_args = 3,
	.register_mode = registered_only
};
