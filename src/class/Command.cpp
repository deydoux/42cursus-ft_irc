#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Channel.hpp"
#include "class/Server.hpp"

#include <sstream>

void Command::init()
{
	_commands["ping"] = (_command_t) {&_ping, 1, 1, true};
	_commands["join"] = (_command_t) {&_join, 1, 2, true};
	_commands["motd"] = (_command_t) {&_motd, 0, 1, true};
	_commands["quit"] = (_command_t) {&_quit, 0, 1, true};
	_commands["mode"] = (_command_t) {&_mode, 2, 3, true};
	_commands["nick"] = (_command_t) {&_nick, 1, 1, false};
	_commands["pass"] = (_command_t) {&_pass, 1, 1, false};
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
	client.set_realname(args[4]);
}

void Command::_join(const args_t &args, Client &client)
{
	size_t args_size = args.size();

	// if the only argument to /join 0
	if (args_size == 2 && args[1] == "0")
		// TODO close_all_chanels() -> would be easier to implement when the /PART command will be
		return ;

	std::vector<Channel *> channels_to_be_joined;
	std::vector<std::string> passkeys;
	Server &server = client.get_server();

	std::vector<std::string> channels_name = ft_split(args[1], ',');
	std::vector<std::string> input_passkeys = args_size == 3 ?
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
		client.join_channel(*channel, passkey);

		args_t response_args;
		response_args.push_back(channel->get_name());
		channel->send_broadcast(client.create_cmd_reply(
			client_mask, "JOIN", response_args
		));
	}
}

void Command::_ping(const args_t &args, Client &client)
{
	args_t response_args;
	response_args.push_back(client.get_server().get_name());
	response_args.push_back(args[1]);

	client.send(client.create_cmd_reply(
		client.get_server().get_name(), "PONG", response_args
	));
}

void Command::_quit(const args_t &args, Client &client)
{
	std::string quit_message = "Client Quit";
	if (args.size() > 1)
		quit_message = args[1];

	args_t response_args;
	response_args.push_back(quit_message);
	
	channels_t client_channels = client.get_active_channels();
	for (channels_t::iterator channel = client_channels.begin(); channel != client_channels.end(); channel++) {
		channel->second->remove_client(client.get_fd());
		channel->second->send_broadcast(client.create_cmd_reply(
			client.get_mask(), "QUIT", response_args
		));
	}
}

void Command::_mode(const args_t &args, Client &client)
{
	std::string target = args[1];
	Server *server = &client.get_server();

	Channel *channel = server->find_channel(target);
	if (!channel)
		// The real message would be "No such nick or ..", but in kittirc, only channel modes can be edied, not the user modes
		return client.reply(ERR_NOSUCHNICK, target, "No such channel name");

	if (args.size() == 2)
	{
		std::string channel_modes = channel->get_modes();
		// If the user is inside the channel and a passkey is required, then it must appears in the reply message
		if (channel_modes.find('k') != std::string::npos && channel->is_client_member(client)) {
			channel_modes += ' ' + channel->get_passkey();
		}

		client.reply(RPL_CHANNELMODEIS, target, channel_modes);
		client.reply(RPL_CREATIONTIME, target, channel->get_creation_timestamp());		
		return ;
	}

	std::stringstream applied_modes;

	bool add_mode = true;
	for (char mode: args[2])
	{
		if (mode == '-') {
			add_mode = false;
			continue ;
		} else if (mode == '+')
			continue ;

		if (std::string("itokl").find(mode) == std::string::npos) {
			client.reply(ERR_UNKNOWNMODE, "" + mode, "is unknown mode char for " + target);
			continue ;
		}
		else if ((mode == 'k' || mode == 'l') && add_mode && args.size() < 4) {
			client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");
			continue ;
		}
		else if (!client.is_channel_op(target)) {
			client.reply(ERR_CHANOPRIVSNEEDED, target, "You are not channel operator");
			continue ;
		}

		std::string argument = args[3];

		if (mode == 'i') {
			if (channel->is_invite_only() == add_mode) continue ;
			channel->set_is_invite_only(add_mode);
		}

		else if (mode == 't') {
			if (channel->is_topic_protected() == add_mode) continue ;
			channel->set_is_topic_protected(add_mode);
		}

		else if (mode == 'k' && add_mode)
			channel->set_passkey(argument);
		
		else if (mode == 'k' && !add_mode) {
			std::string empty_pass = "";
			channel->set_passkey(empty_pass);
		}

		else if (mode == 'l' && add_mode)
			channel->set_max_members(std::atoi(argument.c_str()));
		
		else if (mode  == 'l' && !add_mode)
			channel->unset_members_limit();

		else if (mode == 'o') {
			Client *new_op = server->get_client(argument);
			if (new_op == NULL) {
				client.reply(ERR_NOSUCHNICK, argument, "No such nick or channel name");
				continue ;
			}

			if (!channel->is_client_member(*new_op)) {
				client.reply(ERR_USERNOTINCHANNEL, argument + ' ' + target, "They aren't on that channel");
				continue ;
			}

			if (add_mode)
				new_op->give_op_permissions_to(*channel);
			else
				new_op->remove_op_permissions_from(*channel);
		}

		add_mode = true;
	}


	/*
	syntax: MODE {#<channel>,<nickname>} <+/-><modes> [parameters]

	channel modes: 
	- i: Set/remove invite-only status
	- t: Set/remove topic protection
	- o: Give/take channel operator status
	- k <key>: Set/remove channel password
	- l <limit>: Set/remove user limit

	a few interesting features:
	- no symbol (+/-) means +
	- when a mode is set, it channel broadcasts the mode just set
		<< MODE #the-cure -t
		>> :quteriss!~quteriss@z4r8p5.local MODE #the-cure -t
	- this can also work for a group of modes, in the format that it was sent to the server
		<< MODE #the-cure +it
		>> :quteriss!~quteriss@z4r8p5.local MODE #the-cure +it
	- but when the mode is already set, it returns nothing
	- if the mode used is an unrecognized character, it returns a specific error message (472)
		<< MODE #the-cure +y
		>> :irc.example.net 472 quteriss y :is unknown mode char for #the-cure

	some weird behaviors:
	- when a mode receives a parameters, whe it needed one only in + state, then it does this ...
		<< MODE #the-cure -l 10
		>> :irc.example.net 472 quteriss 1 :is unknown mode char for #the-cure
		>> :irc.example.net 472 quteriss 0 :is unknown mode char for #the-cure
		>> :quteriss!~quteriss@z4r8p5.local MODE #the-cure -l
	- 
		<< MODE #the-cure +i -i i -i i- i i i i i i i
		>> :quteriss!~quteriss@z4r8p5.local MODE #the-cure -i

	<< MODE #test +it -it
	>> :quteriss!~quteriss@z3r3p3.local MODE #test +it-it

	<< MODE #test it -it
	>> :quteriss!~quteriss@z3r3p3.local MODE #test +it-it
	*/
	
}
