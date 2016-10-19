

#ifndef __SGP_FILELOGGER_HEADER__
#define __SGP_FILELOGGER_HEADER__

#include "sgp_Logger.h"
#include "../files/sgp_File.h"
#include "../common/sgp_ScopedPointer.h"


//==============================================================================
/**
    A simple implemenation of a Logger that writes to a file.
*/
class SGP_API  FileLogger  : public Logger
{
public:
    //==============================================================================
    /** Creates a FileLogger for a given file.

        @param fileToWriteTo    the file that to use - new messages will be appended
                                to the file. If the file doesn't exist, it will be created,
                                along with any parent directories that are needed.
        @param welcomeMessage   when opened, the logger will write a header to the log, along
                                with the current date and time, and this welcome message
        @param bAlwaysDelete    if this is true, then the file will always be deleted. Note that
                                the param is only checked once when this object is created - any logging
                                that is done later will be appended without any checking
    */
    FileLogger (const File& fileToWriteTo,
                const String& welcomeMessage,
                const bool bAlwaysDelete = false);

    /** Destructor. */
    virtual ~FileLogger();

    //==============================================================================
    /** Returns the file that this logger is writing to. */
    const File& getLogFile() const noexcept               { return logFile; }

    //==============================================================================
    /** Helper function to create a log file in the correct place for this platform.

        The method might return nullptr if the file can't be created for some reason.

        @param logFileSubDirectoryName      the name of the subdirectory to create inside the logs folder (as
                                            returned by getSystemLogFileFolder). It's best to use something
                                            like the name of your application here.
        @param logFileName                  the name of the file to create, e.g. "MyAppLog.txt".
        @param welcomeMessage               a message that will be written to the log when it's opened.
        @param bAlwaysDelete                (see the FileLogger constructor for more info on this)
    */
    static FileLogger* createDefaultAppLogger (const String& logFileSubDirectoryName,
                                               const String& logFileName,
                                               const String& welcomeMessage,
                                               const bool bAlwaysDelete = false);

    /** Helper function to create a log file in the correct place for this platform.

        The filename used is based on the root and suffix strings provided, along with a
        time and date string, meaning that a new, empty log file will be always be created
        rather than appending to an exising one.

        The method might return nullptr if the file can't be created for some reason.

        @param logFileSubDirectoryName      the name of the subdirectory to create inside the logs folder (as
                                            returned by getSystemLogFileFolder). It's best to use something
                                            like the name of your application here.
        @param logFileNameRoot              the start of the filename to use, e.g. "MyAppLog_". This will have
                                            a timestamp and the logFileNameSuffix appended to it
        @param logFileNameSuffix            the file suffix to use, e.g. ".txt"
        @param welcomeMessage               a message that will be written to the log when it's opened.
    */
    static FileLogger* createDateStampedLogger (const String& logFileSubDirectoryName,
                                                const String& logFileNameRoot,
                                                const String& logFileNameSuffix,
                                                const String& welcomeMessage);

    //==============================================================================
    /** Returns an OS-specific folder where log-files should be stored.

        On Windows this will return a logger with a path such as:
        c:\\Documents and Settings\\username\\Application Data\\[logFileSubDirectoryName]\\[logFileName]

        On the Mac it'll create something like:
        ~/Library/Logs/[logFileSubDirectoryName]/[logFileName]

        @see createDefaultAppLogger
    */
    static File getSystemLogFileFolder();

    // (implementation of the Logger virtual method)
    void logMessage (const String& message, ESGPLOG_LEVEL ll=ELL_INFORMATION);
	void setLogLevel(ESGPLOG_LEVEL _level) { m_LogLevel = _level; }

private:
    //==============================================================================
    File logFile;
    CriticalSection logLock;


    SGP_DECLARE_NON_COPYABLE (FileLogger)
};


#endif   // __SGP_FILELOGGER_HEADER__
