#ifndef LIB_HPP
#define LIB_HPP

#include "to_string.tpp"
#include "text_formatting.hpp"

#include <iostream>
#include <vector>
#include <ctime>

typedef enum {debug, info, warning, error}	log_level;

void						log(const std::string &context, const std::string &message, const log_level level = info);
int							calc_time_left(std::time_t start, int total_seconds);
int							get_max_len(std::vector<std::string> arr);
std::string					format_inline_choices(const std::string &choice1, const std::string &choice2, int space_between);
std::string					format_choice(char letter, const std::string &choice);
std::string					get_ordinal(int n);
std::string					join_strings(const std::vector<std::string>& strings);
std::string					html_decode(const std::string &str);
std::vector<std::string>	ft_split(const std::string &str, char separator);

template <typename T>
std::string to_string(T n);

#endif
