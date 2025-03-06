#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	if (args.size() == 1)
		return client.reply(ERR_NORECIPIENT, "", "No recipient given (" + args[0] + ")");

	if (args.size() == 2)
		return client.reply(ERR_NOTEXTTOSEND, "", "No text to send");

	std::vector<std::string> recipients = ft_split(args[1], ',');
	const std::string &message = args[2];

	for (std::vector<std::string>::iterator it = recipients.begin(); it != recipients.end(); it++) {
		const std::string &recipient = *it;

		if (Channel::is_prefix(recipient[0])) {
			Channel *channel = server.get_channel(recipient);

			if (!channel)
				client.reply(ERR_NOSUCHNICK, recipient, "No such nick or channel name");

			else if (!channel->is_client_member(client))
				client.reply(ERR_NOTONCHANNEL, recipient, "You are not on that channel");

			else {
				std::string reply = Client::create_cmd_reply(client.get_mask(), "PRIVMSG", channel->get_name(), message);
				channel->send_broadcast(reply, client.get_fd());
			}
		}

		else {
			Client *target = server.get_client(recipient);

			if (!target)
				return client.reply(ERR_NOSUCHNICK, recipient, "No such nick or channel name");

			target->cmd_reply(client.get_mask(), "PRIVMSG", recipient, message);
		}
	}
}

const Command::_command_t Command::_privmsg = {
	.handler = &handler,
	.min_args = 0,
	.max_args = 2,
	.register_mode = registred_only
};
