#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::init()
{
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
