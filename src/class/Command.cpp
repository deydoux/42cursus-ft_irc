#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <cstdlib>
#include <ctime>

void Command::init()
{
	_commands["hk"] = _hk;
	_commands["invite"] = _invite;
	_commands["join"] = _join;
	_commands["kick"] = _kick;
	_commands["mode"] = _mode;
	_commands["motd"] = _motd;
	_commands["names"] = _names;
	_commands["nick"] = _nick;
	_commands["part"] = _part;
	_commands["pass"] = _pass;
	_commands["ping"] = _ping;
	_commands["privmsg"] = _privmsg;
	_commands["quit"] = _quit;
	_commands["topic"] = _topic;
	_commands["user"] = _user;
	_commands["who"] = _who;

	std::srand(std::time(NULL));
}

void Command::execute(const args_t &args, Client &client, Server &server)
{
	_commands_t::iterator command_it = _commands.find(to_lower(args[0]));

	if (command_it == _commands.end()) {
		if (client.is_registered())
			client.reply(ERR_UNKNOWNCOMMAND, args[0], "Unknown command");
		return;
	}

	_command_t command = command_it->second;

	switch (command.register_mode)
	{
	case none:
		break;
	case unregistered_only:
		if (client.is_registered())
			return client.reply(ERR_ALREADYREGISTERED, "", "Connection already registered");
		break;
	case registered_only:
		if (!client.is_registered())
			return client.reply(ERR_NOTREGISTERED, "", "Connection not registered");
		break;
	}

	size_t args_size = args.size() - 1;
	if (args_size > command.max_args || args_size < command.min_args)
		return client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");

	command.handler(args, client, server);
}

Command::_commands_t Command::_commands;
