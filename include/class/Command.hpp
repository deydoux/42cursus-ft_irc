#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "lib.hpp"

class Client;

class Command
{
public:
	static void init();
	static void execute(const args_t &args, Client &client);
private:
	typedef void	(*_command_handler_t)(const args_t &, Client &);
	typedef struct {
		_command_handler_t	handler;
		size_t				min_args;
		size_t				max_args;
		bool				need_registration;
	}	_command_t;
	typedef std::map<std::string, _command_t>	_commands_t;

	static _commands_t	_commands;

	static void	_nick(const args_t &args, Client &client);
	static void	_pass(const args_t &args, Client &client);
	static void	_user(const args_t &args, Client &client);
	static void	_ping(const args_t &args, Client &client);
};

#endif // COMMAND_HPP
