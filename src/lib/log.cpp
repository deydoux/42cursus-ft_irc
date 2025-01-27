#include "color.h"
#include "log.h"

#include <iostream>

void log(const std::string &context, const std::string &message, const log_level level) {
	std::string level_str;
	switch (level) {
		case debug:
			level_str = MAG "DEBUG" RESET;
			break;
		case info:
			level_str = BLU "INFO" RESET;
			break;
		case warning:
			level_str = BYEL "WARNING" RESET;
			break;
		case error:
			level_str = BRED "ERROR" RESET;
			break;
	}

	std::cerr << '[' << level_str << "][" << context << "] " << message << std::endl;
}
