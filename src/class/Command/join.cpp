#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	size_t args_size = args.size();

	// if the only argument to /join 0
	if (args_size == 2 && args[1] == "0")
		// TODO close_all_chanels() -> would be easier to implement when the /PART command will be
		return ;

	std::vector<Channel *> channels_to_be_joined;
	std::vector<std::string> channel_names_to_be_joined;
	std::vector<std::string> passkeys;

	std::vector<std::string> channels_name = ft_split(args[1], ',');
	std::vector<std::string> input_passkeys = args_size == 3 ? ft_split(args[2], ',') : std::vector<std::string>();

	for (size_t i = 0; i < channels_name.size(); i++)
	{
		std::string channel_name = channels_name[i];
		if (Channel::is_valid_name(channel_name)) {
			Channel *new_channel = server.get_channel(channel_name);
			if (!new_channel) {
				new_channel = new Channel(client, channel_name, server.is_verbose());
				server.add_channel(*new_channel);
			}

			channels_to_be_joined.push_back(new_channel);
			channel_names_to_be_joined.push_back(channel_name);

			if (args_size == 3)
				passkeys.push_back(input_passkeys.size() > i ? input_passkeys[i] : "");

		} else {
			client.reply(ERR_NOSUCHCHANNEL, channels_name[i], "No such channel");
		}
	}

	std::string client_mask = client.get_mask();

	for (size_t i = 0; i < channels_to_be_joined.size(); i++)
	{
		Channel *channel = channels_to_be_joined[i];
		std::string &channel_name = channel_names_to_be_joined[i];

		std::string passkey = args_size == 3 && passkeys.size() > i ? passkeys[i] : "";

		if (client.join_channel(*channel, passkey)) {
			channel->send_broadcast(
				Client::create_cmd_reply(client_mask, "JOIN", "", channel_name)
			);

			args_t names_args;
			names_args.push_back("NAMES");
			names_args.push_back(channel_name);

			Command::execute(names_args, client, server);
		}
	}
}

const Command::_command_t Command::_join = {
	.handler = &handler,
	.min_args = 1,
	.max_args = 2,
	.register_mode = registred_only
};
