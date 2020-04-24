#ifndef CONSOLE_LOGGER
#define CONSOLE_LOGGER

#include "game/headers/utility/logger.hh"

#include <string_view>

class ConsoleLogger : public Logger {
	void Info(std::string_view message) override;
	void Warning(std::string_view message) override;
	void Error(std::string_view message) override;
};

#endif // CONSOLE_LOGGER
