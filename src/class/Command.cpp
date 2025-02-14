#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Channel.hpp"
#include "class/Server.hpp"

void Command::init()
{
	_commands["join"] = (_command_t) {&_join, 1, 2, true};
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

	size_t args_size(args.size() - 1);
	if (args_size > command.max_args || args_size < command.min_args)
		return client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");

	if (command.need_registration && !client.is_registered())
		return client.reply(ERR_NOTREGISTERED, "", "Connection not registered");

	command.handler(args, client);
}

Command::_commands_t Command::_commands;

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

	// if the only argument to /join is 0
	if (args_size == 2 && args[1] == "0")
		// TODO close_all_chanels()
		return ;

	std::vector<Channel> channels_to_be_joined;
	std::vector<std::string> passkeys;
	Server server = client.get_server();
	
	std::vector<std::string> channels_name = ft_split(args[1], ',');
	std::vector<std::string> input_passkeys = args_size == 3 ? 
		ft_split(args[2], ',') : std::vector<std::string>({});

	for (size_t i = 0; i < channels_name.size(); i++)
	{
		std::string channel_name = channels_name[i];
		if (Channel::is_valid_name(channel_name)) {

			try {
				// fetch existing channel
				channels_to_be_joined.push_back(server.find_channel(channel_name));
			} catch (std::invalid_argument &) {
				// the channel does not exists and needs to be created
				Channel new_channel = Channel(client, channel_name);

				channels_to_be_joined.push_back(new_channel);
				server.add_channel(new_channel);
			}

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

	for (size_t i = 0; i < channels_to_be_joined.size(); i++)
	{
		Channel channel = channels_to_be_joined[i];
		std::string passkey = args_size == 3 ? passkeys[i] : "";

		// ERR_CHANNELISFULL => must check the max number of users on the channel and if there's some spaces left
		if (channel.is_full())
			client.reply(ERR_CHANNELISFULL, channel.get_name(), "Cannot join channel (+l)");

		// ERR_BADCHANNELKEY => must check if the channel requires a passkey and if so, if the one given is correct
		else if (!channel.check_passkey(passkey))
			client.reply(ERR_BADCHANNELKEY, channel.get_name(), "Cannot join channel (+k) - bad key");

		// ERR_INVITEONLYCHAN => if the channel is INVITE-ONLY, check if the user has been invited
		if (channel.is_invite_only() && !client.is_invited_to(channel))
			client.reply(ERR_INVITEONLYCHAN, channel.get_name(), "Cannot join channel (+i)");

		// ERR_BANNEDFROMCHAN => check if the user has not been banned from this channel
		if (channel.is_client_banned(client))
			client.reply(ERR_BANNEDFROMCHAN, channel.get_name(), "Cannot join channel (+b)");

}
