
ConsoleLogger::ConsoleLogger(const String& welcomeMessage)
{
    String welcome;
    welcome << newLine
            << "**********************************************************" << newLine
            << welcomeMessage << newLine
            << "Log started: " << Time::getCurrentTime().toString (true, true) << newLine;

    logMessage( welcome, ELL_NONE );
}

ConsoleLogger::~ConsoleLogger() {}

void ConsoleLogger::logMessage (const String& message, ESGPLOG_LEVEL ll)
{
	if( ll < m_LogLevel )
		return;

    DBG (message);
	wprintf( L"%s\n", message.toWideCharPointer() );
}