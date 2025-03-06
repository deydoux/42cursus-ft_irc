#ifndef LIB_HPP
#define LIB_HPP

#include "to_string.tpp"

#include <iostream>
#include <vector>

typedef enum {debug, info, warning, error}	log_level;

void	log(const std::string &context, const std::string &message, const log_level level = info);

template <typename T>
std::string to_string(T n);

#endif
