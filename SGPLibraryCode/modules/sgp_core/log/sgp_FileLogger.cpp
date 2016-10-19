
FileLogger::FileLogger (const File& file,
                        const String& welcomeMessage,
                        const bool bAlwaysDelete)
    : logFile (file)
{
    if( bAlwaysDelete )
	{
		logFile.deleteFile();
	}

    if (! file.exists())
        file.create();  // (to create the parent directories)

    String welcome;
    welcome << newLine
            << "**********************************************************" << newLine
            << welcomeMessage << newLine
            << "Log started: " << Time::getCurrentTime().toString (true, true) << newLine;

    logMessage( welcome, ELL_NONE );
}

FileLogger::~FileLogger() {}

//==============================================================================
void FileLogger::logMessage (const String& message, ESGPLOG_LEVEL ll)
{
	if( ll < m_LogLevel )
		return;
    const ScopedLock sl (logLock);
    DBG (message);
    FileOutputStream out (logFile, 256);
    out << message << newLine;
}


//==============================================================================
File FileLogger::getSystemLogFileFolder()
{
   #if SGP_MAC
    return File ("~/Library/Logs");
   #else
    return File::getSpecialLocation (File::userApplicationDataDirectory);
   #endif
}

FileLogger* FileLogger::createDefaultAppLogger (const String& logFileSubDirectoryName,
                                                const String& logFileName,
                                                const String& welcomeMessage,
                                                const bool    bAlwaysDelete)
{
    return new FileLogger (getSystemLogFileFolder().getChildFile (logFileSubDirectoryName)
                                                   .getChildFile (logFileName),
                           welcomeMessage, bAlwaysDelete);
}

FileLogger* FileLogger::createDateStampedLogger (const String& logFileSubDirectoryName,
                                                 const String& logFileNameRoot,
                                                 const String& logFileNameSuffix,
                                                 const String& welcomeMessage)
{
    return new FileLogger (getSystemLogFileFolder().getChildFile (logFileSubDirectoryName)
                                                   .getChildFile (logFileNameRoot + Time::getCurrentTime().formatted ("%Y-%m-%d_%H-%M-%S"))
                                                   .withFileExtension (logFileNameSuffix)
                                                   .getNonexistentSibling(),
                           welcomeMessage, 0);
}
