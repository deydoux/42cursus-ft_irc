#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void join_handler(const args_t &args, Client &client, Server &server)
{
	size_t args_size = args.size();

	if (args_size == 2 && args[1] == "0") {
		std::string reason = client.get_nickname();

		for (channels_t::const_iterator it = client.get_channels().begin(); it != client.get_channels().end(); ++it) {
			Channel &channel = *it->second;
			client.part(channel, reason);
		}

		return;
	}

	std::vector<Channel *> channels_to_join;
	std::vector<std::string> channels_name_to_join;
	std::vector<std::string> passkeys;

	std::vector<std::string> channels_name = split(args[1], ',');
	std::vector<std::string> input_passkeys = args_size == 3 ? split(args[2], ',') : std::vector<std::string>();

	for (size_t i = 0; i < channels_name.size(); ++i)
	{
		std::string channel_name = channels_name[i];
		if (Channel::is_valid_name(channel_name)) {
			Channel *new_channel = server.get_channel(channel_name);
			if (!new_channel) {
				new_channel = new Channel(client, channel_name, server.is_verbose());
				server.add_channel(*new_channel);
			}

			channels_to_join.push_back(new_channel);
			channels_name_to_join.push_back(channel_name);

			if (args_size == 3)
				passkeys.push_back(input_passkeys.size() > i ? input_passkeys[i] : "");

		} else {
			client.reply(ERR_NOSUCHCHANNEL, channels_name[i], "No such channel");
		}
	}

	std::string client_mask = client.get_mask();

	for (size_t i = 0; i < channels_to_join.size(); ++i) {
		Channel *channel = channels_to_join[i];
		std::string &channel_name = channels_name_to_join[i];
		std::string passkey = args_size == 3 && passkeys.size() > i ? passkeys[i] : "";

		client.join(channel_name, *channel, passkey);
	}
}

const Command::_command_t Command::_join = {
	.handler = &join_handler,
	.min_args = 1,
	.max_args = 2,
	.register_mode = registered_only
};
