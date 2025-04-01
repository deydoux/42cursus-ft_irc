#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void list_handler(const args_t &args, Client &client, Server &server)
{
	if (args.size() == 3 && args[2] != server.get_name())
		return client.reply(ERR_NOSUCHSERVER, args[2], "No such server");

	const std::string &mask = args.size() == 1 ? "*" : args[1];
	const channels_t &channels = server.get_channels(mask);

	std::string reply = client.create_reply(RPL_LISTSTART, "Channel", "Users Name");

	for (channels_t::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		const Channel &channel = *it->second;

		const std::string &member_count_str = to_string(channel.get_members().size());
		reply += client.create_reply(RPL_LIST, channel.get_name() + ' ' + member_count_str, channel.get_topic(), true);
	}

	reply += client.create_reply(RPL_LISTEND, "", "End of LIST");
	client.send(reply);
}

const Command::_command_t Command::_list = {
	.handler = &list_handler,
	.min_args = 0,
	.max_args = 2,
	.register_mode = registered_only
};
