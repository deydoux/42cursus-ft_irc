#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

static void handler(const args_t &args, Client &client, Server &server)
{
	std::string channel_name = args[1];
	Channel *channel = server.get_channel(channel_name);

	if (!channel)
		return client.reply(ERR_NOSUCHCHANNEL, channel_name, "No such channel");

	if (!channel->is_client_member(client))
		return client.reply(ERR_NOTONCHANNEL, channel_name, "You are not on that channel");

	channel_name = channel->get_name();
	std::string channel_topic = channel->get_topic();

	if (args.size() == 2)
	{
		if (channel_topic.empty())
			return client.reply(RPL_NOTOPIC, channel_name, "No topic is set");

		std::string reply = client.create_reply(RPL_TOPIC, channel_name, channel_topic);
		reply += client.create_reply(RPL_TOPICWHOTIME, channel_name + " " + channel->get_topic_last_edited_by(), channel->get_topic_last_edited_at());

		client.send(reply);
		return;
	}

	if (channel->is_topic_protected() && !client.is_channel_operator(channel_name))
		return client.reply(ERR_CHANOPRIVSNEEDED, channel_name, "You are not channel operator");

	std::string new_topic = args[2];
	if (new_topic.size() > Channel::max_topic_len)
		new_topic.resize(Channel::max_topic_len);

	channel->set_topic(client, new_topic);
	channel->send_broadcast(
		client.create_cmd_reply(client.get_mask(), "TOPIC", channel_name, new_topic)
	);
}

const Command::_command_t Command::_topic = {
	.handler = &handler,
	.min_args = 1,
	.max_args = 2,
	.register_mode = registered_only
};
