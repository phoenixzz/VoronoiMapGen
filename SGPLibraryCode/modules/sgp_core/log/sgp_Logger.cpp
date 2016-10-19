
Logger::Logger() { m_LogLevel = ELL_NONE; }

Logger::~Logger()
{
    // You're deleting this logger while it's still being used!
    // Always call Logger::setCurrentLogger (nullptr) before deleting the active logger.
    jassert (currentLogger != this);
}

Logger* Logger::currentLogger = nullptr;

void Logger::setCurrentLogger (Logger* const newLogger) noexcept    { currentLogger = newLogger; }
Logger* Logger::getCurrentLogger()  noexcept                        { return currentLogger; }

void Logger::writeToLog (const String& message, ESGPLOG_LEVEL ll)
{
    if (currentLogger != nullptr)
        currentLogger->logMessage (message, ll);
    else
        outputDebugString (message);
}


