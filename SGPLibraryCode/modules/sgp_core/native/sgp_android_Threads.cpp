
/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in sgp_posix_SharedCode.h!
*/

//==============================================================================
// sets the process to 0=low priority, 1=normal, 2=high, 3=realtime
void Process::setPriority (ProcessPriority prior)
{
    // TODO

    struct sched_param param;
    int policy, maxp, minp;

    const int p = (int) prior;

    if (p <= 1)
        policy = SCHED_OTHER;
    else
        policy = SCHED_RR;

    minp = sched_get_priority_min (policy);
    maxp = sched_get_priority_max (policy);

    if (p < 2)
        param.sched_priority = 0;
    else if (p == 2 )
        // Set to middle of lower realtime priority range
        param.sched_priority = minp + (maxp - minp) / 4;
    else
        // Set to middle of higher realtime priority range
        param.sched_priority = minp + (3 * (maxp - minp) / 4);

    pthread_setschedparam (pthread_self(), policy, &param);
}

void Process::terminate()
{
    // TODO
    exit (0);
}

SGP_API bool SGP_CALLTYPE sgp_isRunningUnderDebugger()
{
    return false;
}

SGP_API bool SGP_CALLTYPE Process::isRunningUnderDebugger()
{
    return sgp_isRunningUnderDebugger();
}

void Process::raisePrivilege() {}
void Process::lowerPrivilege() {}
