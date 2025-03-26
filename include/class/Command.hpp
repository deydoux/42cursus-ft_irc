#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "lib.hpp"

class Client;

class Command
{
private:
// Types
	typedef void	(*_command_handler_t)(const args_t &, Client &, Server &);
	typedef enum {
		none,
		unregistered_only,
		registered_only
	}	_register_mode_t;
	typedef struct {
		_command_handler_t	handler;
		size_t				min_args;
		size_t				max_args;
		_register_mode_t	register_mode;
	}	_command_t;
	typedef std::map<std::string, _command_t>	_commands_t;

// Static variables
	static const _command_t	_hk;
	static const _command_t	_invite;
	static const _command_t	_join;
	static const _command_t	_kick;
	static const _command_t	_list;
	static const _command_t	_mode;
	static const _command_t	_motd;
	static const _command_t	_names;
	static const _command_t	_nick;
	static const _command_t	_part;
	static const _command_t	_pass;
	static const _command_t	_ping;
	static const _command_t	_privmsg;
	static const _command_t	_quit;
	static const _command_t	_topic;
	static const _command_t	_user;
	static const _command_t	_who;

	static _commands_t	_commands;

public:
// Static functions
	static void init();
	static void execute(const args_t &args, Client &client, Server &server);
};

#endif // COMMAND_HPP
