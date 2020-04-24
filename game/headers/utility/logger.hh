#ifndef LOGGER_HH
#define LOGGER_HH

#include <string_view>

class Logger {
public:
	virtual void Info(std::string_view message) = 0;
	virtual void Warning(std::string_view message) = 0;
	virtual void Error(std::string_view message) = 0;

	virtual ~Logger() {};
};

#endif // LOGGER_HH
