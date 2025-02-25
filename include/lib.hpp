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

typedef std::map<int, Client *>				clients_t; //	{client fd, client}
typedef std::map<std::string, Channel *>	channels_t; //	{channel name, channel}
typedef std::vector<std::string>			args_t;

typedef enum {debug, info, warning, error}	log_level;

void	log(const std::string &context, const std::string &message, const log_level level = info);

std::string		to_lower(const std::string &str);
bool			match_mask(const std::string& mask, const std::string& user_string);
std::vector<std::string> ft_split(const std::string &str, char separator);
std::string		unix_timestamp( void );

template <typename T>
std::string to_string(T n);

#endif // LIB_HPP
