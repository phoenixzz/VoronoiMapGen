#ifndef __SGP_CONSOLELOGGER_HEADER__
#define __SGP_CONSOLELOGGER_HEADER__

#include "sgp_Logger.h"

//==============================================================================
/**
    A simple implemenation of a Logger that writes to console.
*/
class SGP_API  ConsoleLogger  : public Logger
{
public:
	/*
        @param welcomeMessage   when opened, the logger will write a welcome message to the console,
								along with the current date and time 
	*/
	ConsoleLogger(const String& welcomeMessage);
	virtual ~ConsoleLogger();

	// (implementation of the Logger virtual method)
	void logMessage (const String& message, ESGPLOG_LEVEL ll=ELL_INFORMATION);
	void setLogLevel( ESGPLOG_LEVEL _level ) { m_LogLevel = _level; }

private:
	SGP_DECLARE_NON_COPYABLE (ConsoleLogger)
};
#endif		// __SGP_CONSOLELOGGER_HEADER__