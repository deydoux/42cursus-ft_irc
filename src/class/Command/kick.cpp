#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void kick_handler(const args_t &args, Client &client, Server &server)
{
	std::vector<std::string> channels_name = split(args[1], ',');
	std::vector<std::string> nicks_to_kick = split(args[2], ',');
	std::string reason = args.size() == 4 ? args[3] : client.get_nickname();

	std::vector<Channel *> kick_from_channels;

	for (size_t i = 0; i < channels_name.size(); ++i) {
		std::string channel_name = channels_name[i];
		Channel *channel = server.get_channel(channel_name);

		if (!channel)
			client.reply(ERR_NOSUCHCHANNEL, channel_name, "No such channel");
		else
			kick_from_channels.push_back(channel);
	}

	if (reason.size() > Client::max_kick_reason_len)
		reason.resize(Client::max_kick_reason_len);

	for (size_t i = 0; i < nicks_to_kick.size(); ++i)
		for (size_t j = 0; j < kick_from_channels.size(); ++j)
			client.kick(nicks_to_kick[i], *kick_from_channels[j], reason);
}

const Command::_command_t Command::_kick = {
	.handler = &kick_handler,
	.min_args = 2,
	.max_args = 3,
	.register_mode = registered_only
};
