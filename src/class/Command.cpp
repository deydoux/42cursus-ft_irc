#include "class/Command.hpp"

Command::Command()
{
	_commands["invite"] = &Command::_invite;
	_commands["join"] = &Command::_join;
	_commands["kick"] = &Command::_kick;
	_commands["mode"] = &Command::_mode;
	_commands["nick"] = &Command::_nick;
	_commands["pass"] = &Command::_pass;
	_commands["ping"] = &Command::_ping;
	_commands["privmsg"] = &Command::_privmsg;
	_commands["topic"] = &Command::_topic;
	_commands["user"] = &Command::_user;
	_commands["who"] = &Command::_who;
}

void Command::execute(const args_t &args, Client &client)
{
	std::string name = to_lower(args[0]);

	_command_t command = _commands[name];
	if (!command)
		return client.log("Unknown command: " + name, error);
	command(args, client);
}

Command::_commands_t Command::_commands;

void Command::_dummy_command(const args_t &args, Client &client)
{
	client.log("Command not implemented: " + args[0], error);
}

void Command::_invite(const args_t &args, Client &client)
{
	client.log("TODO command invite", warning);
	(void)args;
}

void Command::_join(const args_t &args, Client &client)
{
	client.log("TODO command join", warning);
	(void)args;
}

void Command::_kick(const args_t &args, Client &client)
{
	client.log("TODO command kick", warning);
	(void)args;
}

void Command::_mode(const args_t &args, Client &client)
{
	client.log("TODO command mode", warning);
	(void)args;
}

void Command::_nick(const args_t &args, Client &client)
{
	client.log("TODO command nick", warning);
	(void)args;
}

void Command::_pass(const args_t &args, Client &client)
{
	client.log("TODO command pass", warning);
	(void)args;
}

void Command::_ping(const args_t &args, Client &client)
{
	client.log("TODO command ping", warning);
	(void)args;
}

void Command::_privmsg(const args_t &args, Client &client)
{
	client.log("TODO command privmsg", warning);
	(void)args;
}

void Command::_topic(const args_t &args, Client &client)
{
	client.log("TODO command topic", warning);
	(void)args;
}

void Command::_user(const args_t &args, Client &client)
{
	client.log("TODO command user", warning);
	(void)args;
}

void Command::_who(const args_t &args, Client &client)
{
	client.log("TODO command who", warning);
	(void)args;
}

