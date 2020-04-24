#include "game/headers/utility/console-logger.hh"

#include <iostream>

void ConsoleLogger::Info(std::string_view message) {
	std::clog << "Info: " << message << std::endl;
}

void ConsoleLogger::Warning(std::string_view message) {
	std::clog << "Warning: " << message << std::endl;
}

void ConsoleLogger::Error(std::string_view message) {
	std::cerr << "Error: " << message << std::endl;
}
