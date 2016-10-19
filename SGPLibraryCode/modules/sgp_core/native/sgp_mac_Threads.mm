

/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in sgp_posix_SharedCode.h!
*/

//==============================================================================
bool Process::isForegroundProcess()
{
   #if SGP_MAC
    return [NSApp isActive];
   #else
    return true; // xxx change this if more than one app is ever possible on iOS!
   #endif
}

void Process::makeForegroundProcess()
{
   #if SGP_MAC
    [NSApp activateIgnoringOtherApps: YES];
   #endif
}

void Process::raisePrivilege()
{
    jassertfalse;
}

void Process::lowerPrivilege()
{
    jassertfalse;
}

void Process::terminate()
{
    exit (0);
}

void Process::setPriority (ProcessPriority)
{
    // xxx
}


//==============================================================================
SGP_API bool SGP_CALLTYPE sgp_isRunningUnderDebugger()
{
    static char testResult = 0;

    if (testResult == 0)
    {
        struct kinfo_proc info;
        int m[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
        size_t sz = sizeof (info);
        sysctl (m, 4, &info, &sz, 0, 0);
        testResult = ((info.kp_proc.p_flag & P_TRACED) != 0) ? 1 : -1;
    }

    return testResult > 0;
}

SGP_API bool SGP_CALLTYPE Process::isRunningUnderDebugger()
{
    return sgp_isRunningUnderDebugger();
}
