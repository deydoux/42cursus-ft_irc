#ifndef LIB_HPP
#define LIB_HPP

#ifndef VERSION
# define VERSION "unknown"
#endif

#include "reply_code.hpp"
#include "to_string.tpp"

#include <map>
#include <string>
#include <vector>

class Channel;
class Client;

typedef std::map<int, Client *>				clients_t;
typedef std::map<std::string, Channel *>	channels_t;
typedef std::vector<std::string>			args_t;

typedef enum {debug, info, warning, error}	log_level;

void	log(const std::string &context, const std::string &message, const log_level level = info);

std::string	to_lower(const std::string &str);

template <typename T>
std::string to_string(T n);

#endif // LIB_HPP
