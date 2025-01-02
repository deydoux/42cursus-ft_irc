#include "color.h"
#include "log.h"

#include <iostream>

void program_log(const std::string &message, const log_level level) {
	std::string prefix;
	switch (level) {
		case debug:
			prefix = MAG "DEBUG" RESET;
			break;
		case info:
			prefix = BLU "INFO" RESET;
			break;
		case warning:
			prefix = YEL "WARNING" RESET;
			break;
		case error:
			prefix = BRED "ERROR" RESET;
			break;
	}

	std::cerr << prefix << "\t" << message << std::endl;
}
