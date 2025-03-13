#ifndef LIB_HPP
#define LIB_HPP

#include "to_string.tpp"
#include "text_formatting.hpp"

#include <iostream>
#include <vector>

typedef enum {debug, info, warning, error}	log_level;

std::string					html_decode(const std::string &str);
std::vector<std::string>	ft_split(const std::string &str, char separator);
void						log(const std::string &context, const std::string &message, const log_level level = info);

template <typename T>
std::string to_string(T n);

#endif
