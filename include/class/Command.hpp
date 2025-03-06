#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "lib.hpp"

class Client;

class Command
{
public:
	static void init();
	static void execute(const args_t &args, Client &client, Server &server);
private:
	typedef void	(*_command_handler_t)(const args_t &, Client &, Server &);

	typedef struct {
		_command_handler_t	handler;
		size_t				min_args;
		size_t				max_args;
		bool				need_registration;
	}	_command_t;

	typedef std::map<std::string, _command_t>	_commands_t;

	static _commands_t	_commands;

	static const _command_t	_invite;
	static const _command_t	_join;
	static const _command_t	_kick;
	static const _command_t	_motd;
	static const _command_t	_nick;
	static const _command_t	_pass;
	static const _command_t	_ping;
	static const _command_t	_privmsg;
	static const _command_t	_quit;
	static const _command_t	_mode;
	static const _command_t	_user;
	static const _command_t	_topic;
	static const _command_t	_who;
	static const _command_t	_hk;
	static const _command_t	_names;
};

#endif // COMMAND_HPP
