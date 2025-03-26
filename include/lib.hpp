#ifndef LIB_HPP
#define LIB_HPP

#ifndef VERSION
# define VERSION "unknown"
#endif

#include "reply_code.hpp"

#include <map>
#include <string>
#include <vector>

class Channel;
class Client;

// Types
typedef enum {
	debug,
	info,
	warning,
	error
}	log_level;
typedef std::map<int, Client *>	clients_t; // clients_t[fd] = Client *
typedef std::map<std::string, Channel *>	channels_t; // channels_t[name] = Channel *
typedef std::vector<std::string>	args_t;

void	log(const std::string &context, const std::string &message, const log_level level = info);
std::string	to_lower(const std::string &str);
std::vector<std::string>	split(const std::string &str, char separator);

template <typename T>
std::string	to_string(T n);

#include "to_string.tpp"

#endif // LIB_HPP
