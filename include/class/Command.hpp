#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "lib.hpp"
#include "Server.hpp"

#include <map>

class Client;

class Command
{
public:
	Command();

	static void execute(const args_t &args, Client &client);
private:
	typedef void	(*_command_t)(const args_t &, Client &);
	typedef std::map<std::string, _command_t>	_commands_t;

	static _commands_t	_commands;

	static void _dummy_command(const args_t &args, Client &client);
	static void _invite(const args_t &args, Client &client);
	static void _join(const args_t &args, Client &client);
	static void _kick(const args_t &args, Client &client);
	static void _mode(const args_t &args, Client &client);
	static void _nick(const args_t &args, Client &client);
	static void _pass(const args_t &args, Client &client);
	static void _ping(const args_t &args, Client &client);
	static void _privmsg(const args_t &args, Client &client);
	static void _topic(const args_t &args, Client &client);
	static void _user(const args_t &args, Client &client);
	static void _who(const args_t &args, Client &client);
};

#endif // COMMAND_HPP
