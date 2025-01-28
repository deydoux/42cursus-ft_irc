#ifndef LIB_HPP
#define LIB_HPP

#include <string>
#include <vector>

typedef std::vector<std::string>	args_t;

typedef enum {debug, info, warning, error}	log_level;

void	log(const std::string &context, const std::string &message, const log_level level = info);

std::string	to_string(int n);

#endif // LIB_HPP
