#ifndef LOG_H
#define LOG_H

#include <string>

typedef enum {debug, info, warning, error} log_level;

void log(const std::string &context, const std::string &message, const log_level level = info);

#endif // LOG_H
