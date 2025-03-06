#include "class/Channel.hpp"
#include "class/Client.hpp"
#include "class/Command.hpp"
#include "class/Server.hpp"

void Command::_ping(const args_t &args, Client &client, Server &server)
{
	client.cmd_reply(server.get_name(), "PONG", server.get_name(), args[1]);
}
