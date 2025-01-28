#include "class/Command.hpp"

Command::Command()
{
	_commands["invite"] = &Command::_dummy_command;
	_commands["join"] = &Command::_dummy_command;
	_commands["kick"] = &Command::_dummy_command;
	_commands["mode"] = &Command::_dummy_command;
	_commands["nick"] = &Command::_dummy_command;
	_commands["pass"] = &Command::_dummy_command;
	_commands["ping"] = &Command::_dummy_command;
	_commands["privmsg"] = &Command::_dummy_command;
	_commands["topic"] = &Command::_dummy_command;
	_commands["user"] = &Command::_dummy_command;
	_commands["who"] = &Command::_dummy_command;
}

void Command::execute(const args_t &args, Client &client)
{
	_command_t command = _commands[args[0]];
	if (!command)
		return client.log("Unknown command: " + args[0], error);
	command(args, client);
}

Command::_commands_t Command::_commands;

void Command::_dummy_command(const args_t &args, Client &client)
{
	client.log("Command not implemented: " + args[0], error);
}
