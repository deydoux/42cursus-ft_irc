#ifndef TEXT_FORMATTING_HPP
#define TEXT_FORMATTING_HPP

#include <iostream>

#define BOLD "\x02"
#define ITALIC "\x1D"
#define UNDERLINE "\x1F"
#define RVRSE_COL "\x16"

#define COL_BLUE "\x0302"
#define COL_GREEN "\x0303"
#define COL_RED "\x0304"
#define COL_BROWN "\x0305"
#define COL_PURPLE "\x0306"
#define COL_ORANGE "\x0307"
#define COL_YELLOW "\x0308"
#define COL_LIGHTGREEN "\x0309"
#define COL_CYAN "\x0310"
#define COL_LIGHTCYAN "\x0311"
#define COL_LIGHTBLUE "\x0312"
#define COL_PINK "\x0313"
#define COL_GREY "\x0314"
#define COL_LIGHTGREY "\x0315"

std::string	format(const std::string &str, const std::string &control_chars);

#endif
