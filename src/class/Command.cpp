#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

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
	_commands["topic"] = (_command_t) {&_topic, 1, 2, true};
	_commands["who"] = (_command_t) {&_who, 0, 2, true};
	_commands["hk"] = (_command_t) {&_hk, 0, 1, false};
	_commands["names"] = (_command_t) {&_names, 0, 1, true};

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

	size_t args_size = args.size() - 1;
	if (args_size > command.max_args || args_size < command.min_args)
		return client.reply(ERR_NEEDMOREPARAMS, args[0], "Syntax error");

	if (command.need_registration && !client.is_registered())
		return client.reply(ERR_NOTREGISTERED, "", "Connection not registered");

	command.handler(args, client, server);
}

Command::_commands_t Command::_commands;
