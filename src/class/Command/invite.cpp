#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::_invite(const args_t &args, Client &client, Server &server)
{
	Client *target = server.get_client(args[1]);
	if (!target)
		return client.reply(ERR_NOSUCHNICK, args[1], "No such nick or channel name");

	Channel *channel = server.get_channel(args[2]);

	if (channel) {
		if (!channel->is_client_member(client))
			return client.reply(ERR_NOTONCHANNEL, channel->get_name(), "You are not on that channel");

		if (channel->is_client_member(*target))
			return client.reply(ERR_USERONCHANNEL, target->get_nickname(), "is already on channel");

		channel->invite_client(*target);
	}

	target->cmd_reply(client.get_mask(), "INVITE", args[1], args[2]);
	client.reply(RPL_INVITING, args[1] + ' ' + args[2]);
}
