#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <cstdlib>

static void mode_handler(const args_t &args, Client &client, Server &server)
{
	std::string channel_name = args[1];
	Channel *channel = server.get_channel(channel_name);

	if (!channel)
		return client.reply(ERR_NOSUCHNICK, channel_name, "No such channel name");

	channel_name = channel->get_name();

	if (args.size() == 2) {
		client.reply(RPL_CHANNELMODEIS, channel_name, channel->get_modes(channel->is_client_member(client)));
		client.reply(RPL_CREATIONTIME, channel_name, channel->get_creation_timestamp());
		return ;
	}

	std::vector<std::string> applied_flags;
	std::map<char, std::string> modes_values;
	size_t argument_index = 3;
	char sign, mode;

	sign = '+';
	for (size_t i = 0; i < args[2].size(); i++)
	{
		mode = args[2][i];
		if (mode == '-' || mode == '+') {
			sign = mode;
			continue ;
		}

		if (std::string("itokl").find(mode) == std::string::npos) {
			client.reply(ERR_UNKNOWNMODE, std::string(1, mode), "is unknown mode char for " + channel_name);
			continue ;
		}
		else if ((mode == 'k' || mode == 'l') && sign == '+' && args.size() < argument_index + 1) {
			client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");
			continue ;
		}
		else if (!client.is_channel_operator(channel_name)) {
			client.reply(ERR_CHANOPRIVSNEEDED, channel_name, "You are not channel operator");
			continue ;
		}

		std::string value = args.size() > argument_index ? args[argument_index] : "";
		bool add_mode = sign == '+';

		if (mode == 'i') {
			if (channel->is_invite_only() == add_mode) continue ;
			channel->set_is_invite_only(add_mode);
		}

		else if (mode == 't') {
			if (channel->is_topic_protected() == add_mode) continue ;
			channel->set_is_topic_protected(add_mode);
		}

		else if (mode == 'k') {
			std::string empty_pass = "";
			channel->set_passkey(add_mode ? value : empty_pass);
			modes_values[mode] = add_mode ? value : "*";
		}

		else if (mode == 'l' && add_mode) {
			std::istringstream iss(value);
			int limit;

			iss >> limit;
			if (!iss.eof() || iss.fail() || limit == 0) {
				client.reply(ERR_INVALIDMODEPARAM, channel_name + " l *", "Invalid mode parameter");
				continue;
			}

			channel->set_max_members(limit);
			modes_values[mode] = value;
		} else if (mode == 'l' && !add_mode) {
			channel->unset_members_limit();
			modes_values[mode] = value;
		}

		else if (mode == 'o') {
			if (value.empty()) continue ;

			Client *new_op = server.get_client(value);
			if (!new_op) {
				client.reply(ERR_NOSUCHNICK, value, "No such nick or channel name");
				continue ;
			}

			if (!channel->is_client_member(*new_op)) {
				client.reply(ERR_USERNOTINCHANNEL, value + " " + channel_name, "They aren't on that channel");
				continue ;
			}

			if (add_mode)
				new_op->set_channel_operator(channel_name);
			else
				new_op->remove_channel_operator(channel_name);
		}

		std::string flag = add_mode ? "+" : "-";
		applied_flags.push_back(flag + mode);

		if (mode == 'k' || mode == 'l')
			argument_index++;
	}

	if (!applied_flags.empty())
	{
		Channel::modes_t modes = {
			.flags = applied_flags,
			.values = modes_values
		};

		channel->add_modes(&modes);

		channel->send_broadcast(Client::create_cmd_reply(
			client.get_mask(), "MODE", channel_name + ' ' + Channel::stringify_modes(&modes)
		));
	}
}

const Command::_command_t Command::_mode = {
	.handler = &mode_handler,
	.min_args = 1,
	.max_args = 5,
	.register_mode = registered_only
};
