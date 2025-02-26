#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Channel.hpp"
#include "class/Server.hpp"

#include <sstream>
#include <cstdlib>

void Command::init()
{
	_commands["invite"] = (_command_t) {&_invite, 2, 2, true};
	_commands["join"] = (_command_t) {&_join, 1, 2, true};
	_commands["kick"] = (_command_t) {&_kick, 2, 3, true};
	_commands["motd"] = (_command_t) {&_motd, 0, 1, true};
	_commands["mode"] = (_command_t) {&_mode, 1, 5, true};
	_commands["nick"] = (_command_t) {&_nick, 1, 1, false};
	_commands["pass"] = (_command_t) {&_pass, 1, 1, false};
	_commands["ping"] = (_command_t) {&_ping, 1, 1, true};
	_commands["privmsg"] = (_command_t) {&_privmsg, 0, 2, true};
	_commands["quit"] = (_command_t) {&_quit, 0, 1, true};
	_commands["user"] = (_command_t) {&_user, 4, 4, false};
}

void Command::execute(const args_t &args, Client &client)
{
	_commands_t::iterator command_it = _commands.find(to_lower(args[0]));

	if (command_it == _commands.end()) {
		if (client.is_registered())
			client.reply(ERR_UNKNOWNCOMMAND, args[0], "Unknown command");
		return;
	}

	_command_t command = command_it->second;

	size_t args_size = args.size() - 1;
	if (args_size > command.max_args || args_size < command.min_args)
		return client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");

	if (command.need_registration && !client.is_registered())
		return client.reply(ERR_NOTREGISTERED, "", "Connection not registered");

	command.handler(args, client);
}

Command::_commands_t Command::_commands;

void Command::_invite(const args_t &args, Client &client)
{
	Server &server = client.get_server();

	Client *target = server.get_client(args[1]);
	Channel *channel = server.find_channel(args[2]);

	if (!target)
		return client.reply(ERR_NOSUCHNICK, args[1], "No such nick or channel name");

	if (channel) {
		if (!channel->is_client_member(client))
			return client.reply(ERR_NOTONCHANNEL, channel->get_name(), "You are not on that channel");

		if (channel->is_client_member(*target))
			return client.reply(ERR_USERONCHANNEL, target->get_nickname(), "is already on channel");

		channel->invite_client(*target);
	}

	args_t response_args;
	response_args.push_back(args[1]);
	response_args.push_back(args[2]);

	target->cmd_reply(client.get_mask(), "INVITE", response_args);
	client.reply(RPL_INVITING, args[1] + ' ' + args[2]);
}

void Command::_motd(const args_t &args, Client &client)
{
	if (args.size() == 2 && args[1] != client.get_server().get_name())
		return client.reply(ERR_NOSUCHSERVER, args[1], "No such server");

	client.send(client.create_motd_reply());
}

void Command::_nick(const args_t &args, Client &client)
{
	client.set_nickname(args[1]);
}

void Command::_pass(const args_t &args, Client &client)
{
	client.set_password(args[1]);
}

void Command::_user(const args_t &args, Client &client)
{
	client.set_username(args[1]);
	// TODO: check realname
	client.set_realname(args[4]);
}

void Command::_join(const args_t &args, Client &client)
{
	size_t args_size = args.size();

	// if the only argument to /join 0
	if (args_size == 2 && args[1] == "0")
		// TODO close_all_chanels() -> would be easier to implement when the /PART command will be
		return ;

	std::vector<Channel *> 		channels_to_be_joined;
	std::vector<std::string>	passkeys;
	Server						&server = client.get_server();

	std::vector<std::string>	channels_name = ft_split(args[1], ',');
	std::vector<std::string>	input_passkeys = args_size == 3 ?
		ft_split(args[2], ',') : std::vector<std::string>();

	for (size_t i = 0; i < channels_name.size(); i++)
	{
		std::string channel_name = channels_name[i];
		if (Channel::is_valid_name(channel_name)) {

			// fetch existing channel
			Channel *new_channel = server.find_channel(channel_name);
			if (!new_channel) {
				// the channel does not exists and needs to be created
				new_channel = new Channel(client, channel_name, client.get_server().is_verbose());
				server.add_channel(*new_channel);
			}

			channels_to_be_joined.push_back(new_channel);

			if (args_size == 3)
				passkeys.push_back(input_passkeys.size() > i ? input_passkeys[i] : "");

		} else {
			client.reply(
				ERR_NOSUCHCHANNEL,
				channels_name[i],
				"No such channel"
			);
		}
	}

	std::string client_mask = client.get_mask();

	for (size_t i = 0; i < channels_to_be_joined.size(); i++)
	{
		Channel *channel = channels_to_be_joined[i];

		std::string passkey = args_size == 3 && passkeys.size() > i ? passkeys[i] : "";

		args_t response_args;
		response_args.push_back(channel->get_name());

		if (!client.join_channel(*channel, passkey))
			server.delete_channel(channel->get_name());
		else
		{
			channel->send_broadcast(Client::create_cmd_reply(
			client_mask, "JOIN", response_args
			));
		}
	}
}

void Command::_ping(const args_t &args, Client &client)
{
	args_t response_args;
	response_args.push_back(client.get_server().get_name());
	response_args.push_back(args[1]);

	client.cmd_reply(client.get_server().get_name(), "PONG", response_args);
}

void Command::_quit(const args_t &args, Client &client)
{
	std::string reason = args.size() == 2 ? args[1] : client.get_nickname();
	client.set_quit_reason(reason);

	std::string error_description = args.size() == 2 ? args[1] : "";
	client.send_error('"' + error_description + '"');
}

void Command::_privmsg(const args_t &args, Client &client)
{
	if (args.size() == 1)
		return client.reply(ERR_NORECIPIENT, "", "No recipient given (" + args[0] + ")");

	if (args.size() == 2)
		return client.reply(ERR_NOTEXTTOSEND, "", "No text to send");

	std::vector<std::string> recipients = ft_split(args[1], ',');
	std::string message = args[2];

	for (std::vector<std::string>::iterator it = recipients.begin(); it != recipients.end(); it++) {
		std::string recipient = *it;

		args_t response_args;
		response_args.push_back(recipient);
		response_args.push_back(message);

		if (recipient[0] == '#' || recipient[0] == '&') {
			Channel *channel = client.get_server().find_channel(recipient);

			if (!channel)
				client.reply(ERR_NOSUCHNICK, recipient, "No such nick or channel name");

			else if (!channel->is_client_member(client))
				client.reply(ERR_NOTONCHANNEL, recipient, "You are not on that channel");

			else {
				std::string message = Client::create_cmd_reply(client.get_mask(), "PRIVMSG", response_args);
				channel->send_broadcast(message, client.get_fd());
			}
		}

		else {
			Client *target = client.get_server().get_client(recipient);

			if (!target)
				return client.reply(ERR_NOSUCHNICK, recipient, "No such nick or channel name");

			target->cmd_reply(client.get_mask(), "PRIVMSG", response_args);
		}
	}
}

void Command::_kick(const args_t &args, Client &client)
{
	std::vector<Channel *>		channels_to_kick_from;
	std::string					kicked_client = args[2];
	Server						&server = client.get_server();

	std::vector<std::string>	channels_name = ft_split(args[1], ',');
	std::string 				reason = (args.size() == 4) ? args[3] : client.get_nickname();

	for (size_t i = 0; i < channels_name.size(); i++)
	{
		std::string channel_name = channels_name[i];
		Channel *new_channel = server.find_channel(channel_name);
		if (!new_channel)
		{
			client.reply(
				ERR_NOSUCHCHANNEL,
				channels_name[i],
				"No such channel"
			);
		}
		else
			channels_to_kick_from.push_back(new_channel);
	}

	for (size_t i = 0; i < channels_to_kick_from.size(); i++)
	{
		args_t args;
		args.push_back(channels_to_kick_from[i]->get_name());
		args.push_back(kicked_client);
		args.push_back(reason);

		client.kick_channel(*channels_to_kick_from[i], kicked_client, args);
	}
}

void Command::_mode(const args_t &args, Client &client)
{
	std::string channel_name = args[1];
	Server *server = &client.get_server();

	Channel *channel = server->find_channel(channel_name);
	if (!channel)
		return client.reply(ERR_NOSUCHNICK, channel_name, "No such channel name");

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
			if (add_mode) channel->set_max_members(std::atoi(value.c_str()));
			else channel->unset_members_limit();
			modes_values[mode] = value;
		}

		else if (mode == 'o') {
			if (value.empty()) continue ;

			Client *new_op = server->get_client(value);
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
		args_t response_args;
		response_args.push_back(channel_name);
			
		Channel::modes_t modes = { flags: applied_flags, values: modes_values };

		response_args.push_back(Channel::stringify_modes(&modes));
		channel->add_modes(&modes);

		channel->send_broadcast(Client::create_cmd_reply(
			client.get_mask(), "MODE", response_args
		));
	}
}
