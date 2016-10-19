

#ifndef __SGP_LOGGER_HEADER__
#define __SGP_LOGGER_HEADER__

#include "../text/sgp_String.h"

//! Possible log levels.
//! When used has filter ELL_DEBUG means => log everything 
//! ELL_NONE means => log (nearly) nothing.
//! When used to print logging information ELL_DEBUG will have lowest priority while ELL_NONE
//! messages are never filtered and always printed.
enum ESGPLOG_LEVEL
{
	//! Used for printing information helpful in debugging
	ELL_DEBUG,

	//! Useful information to print. For example hardware infos or something started/stopped.
	ELL_INFORMATION,

	//! Warnings that something isn't as expected and can cause oddities
	ELL_WARNING,

	//! Something did go wrong.
	ELL_ERROR,

	//! Logs with ELL_NONE will never be filtered.
	//! And used as filter it will remove all logging except ELL_NONE messages.
	ELL_NONE
};

//==============================================================================
/**
    Acts as an application-wide logging class.

    A subclass of Logger can be created and passed into the Logger::setCurrentLogger
    method and this will then be used by all calls to writeToLog.

    The logger class also contains methods for writing messages to the debugger's
    output stream.

*/
class SGP_API  Logger
{
public:
    //==============================================================================
    /** Destructor. */
    virtual ~Logger();

    //==============================================================================
    /** Sets the current logging class to use.

        Note that the object passed in will not be owned or deleted by the logger, so
        the caller must make sure that it is not deleted while still being used.
        A null pointer can be passed-in to disable any logging.
    */
    static void SGP_CALLTYPE setCurrentLogger (Logger* newLogger) noexcept;

    /** Returns the current logger, or nullptr if none has been set. */
    static Logger* getCurrentLogger() noexcept;


    //==============================================================================
    /** Writes a message to the standard error stream.

        This can be called directly, or by using the DBG() macro in
        sgp_PlatformDefs.h (which will avoid calling the method in non-debug builds).
    */
    static void SGP_CALLTYPE outputDebugString (const String& text);

    /** Writes a string to the current logger.

        This will pass the string to the logger's logMessage() method if a logger
        has been set.

        @see logMessage
    */
    void SGP_CALLTYPE writeToLog (const String& message, ESGPLOG_LEVEL ll=ELL_INFORMATION);

	virtual void setLogLevel( ESGPLOG_LEVEL _level ) = 0;

protected:
	ESGPLOG_LEVEL	m_LogLevel;
    //==============================================================================
    Logger();

    /** This is overloaded by subclasses to implement custom logging behaviour.
    */
    virtual void logMessage (const String& message, ESGPLOG_LEVEL ll=ELL_INFORMATION) = 0;

private:
    static Logger* currentLogger;
};


#endif   // __SGP_LOGGER_HEADER__
