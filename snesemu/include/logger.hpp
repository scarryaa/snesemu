#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

class Logger
{
public:
	static Logger* getInstance();

	void openLog(const std::string& filename);
	void closeLog();
	void logError(const std::string& message);

	Logger(Logger const&) = delete;
	void operator=(Logger const&) = delete;

	~Logger();
private:
	std::ofstream logFile;
	static Logger* instance;

	Logger();
};

#endif