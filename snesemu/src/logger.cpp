#include "../include/logger.hpp"

Logger* Logger::instance = nullptr;

Logger* Logger::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Logger();
	}
	return instance;
}

void Logger::openLog(const std::string& filename)
{
	// open log file
	logFile.open(filename, std::ios::app);
}

void Logger::logError(const std::string& message)
{
	// write to file if open
	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.flush();
	}
	else
	{
		// throw exception
		throw std::runtime_error("Log file is not open.");
	}
}

void Logger::closeLog()
{
	if (logFile.is_open())
	{
		logFile.close();
	}
}

Logger::~Logger()
{
	closeLog();
	delete instance;
}