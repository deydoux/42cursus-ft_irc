#ifndef TEXT_FORMATTING_HPP
#define TEXT_FORMATTING_HPP

#include <iostream>

#define NO_STYLE "\x0F"
#define BOLD "\x02"
#define ITALIC "\x1D"
#define UNDERLINE "\x1F"
#define RVRSE_COL "\x16"

#define COL_WHITE "\x03""00"
#define COL_BLACK "\x03""01"
#define COL_BLUE "\x03""02"
#define COL_GREEN "\x03""03"
#define COL_RED "\x03""04"
#define COL_BROWN "\x03""05"
#define COL_PURPLE "\x03""06"
#define COL_ORANGE "\x03""07"
#define COL_YELLOW "\x03""08"
#define COL_LIGHTGREEN "\x03""09"
#define COL_CYAN "\x03""10"
#define COL_LIGHTCYAN "\x03""11"
#define COL_LIGHTBLUE "\x03""12"
#define COL_PINK "\x03""13"
#define COL_GREY "\x03""14"
#define COL_LIGHTGREY "\x03""15"

std::string	format(const std::string &str, const std::string &control_chars);

#endif
