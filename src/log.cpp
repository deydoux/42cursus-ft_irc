#include "color.h"
#include "log.h"

#include <iostream>

void log(const std::string &context, const std::string &message, const log_level level) {
	std::string prefix;
	switch (level) {
		case debug:
			prefix = MAG "DEBUG" RESET;
			break;
		case info:
			prefix = BLU "INFO" RESET;
			break;
		case warning:
			prefix = BYEL "WARNING" RESET;
			break;
		case error:
			prefix = BRED "ERROR" RESET;
			break;
	}

	std::cerr << '[' << prefix << "][" << context << "] " << message << std::endl;
}
