

Thread::Thread (const String& threadName_)
    : threadName (threadName_),
      threadHandle (nullptr),
      threadId (0),
      threadPriority (5),
      affinityMask (0),
      shouldExit (false)
{
}

Thread::~Thread()
{
    /* If your thread class's destructor has been called without first stopping the thread, that
       means that this partially destructed object is still performing some work - and that's
       probably a Bad Thing!

       To avoid this type of nastiness, always make sure you call stopThread() before or during
       your subclass's destructor.
    */
    jassert (! isThreadRunning());

    stopThread (100);
}

//==============================================================================
// Use a ref-counted object to hold this shared data, so that it can outlive its static
// shared pointer when threads are still running during static shutdown.
struct CurrentThreadHolder   : public ReferenceCountedObject
{
    CurrentThreadHolder() noexcept {}

    typedef ReferenceCountedObjectPtr <CurrentThreadHolder> Ptr;
    ThreadLocalValue<Thread*> value;

    SGP_DECLARE_NON_COPYABLE (CurrentThreadHolder)
};

static char currentThreadHolderLock [sizeof (SpinLock)]; // (statically initialised to zeros).

static CurrentThreadHolder::Ptr getCurrentThreadHolder()
{
    static CurrentThreadHolder::Ptr currentThreadHolder;
    SpinLock::ScopedLockType lock (*reinterpret_cast <SpinLock*> (currentThreadHolderLock));

    if (currentThreadHolder == nullptr)
        currentThreadHolder = new CurrentThreadHolder();

    return currentThreadHolder;
}

void Thread::threadEntryPoint()
{
    const CurrentThreadHolder::Ptr currentThreadHolder (getCurrentThreadHolder());
    currentThreadHolder->value = this;

    SGP_TRY
    {
        if (threadName.isNotEmpty())
            setCurrentThreadName (threadName);

        if (startSuspensionEvent.wait (10000))
        {
            jassert (getCurrentThreadId() == threadId);

            if (affinityMask != 0)
                setCurrentThreadAffinityMask (affinityMask);

            run();
        }
    }
    SGP_CATCH_ALL_ASSERT

    currentThreadHolder->value.releaseCurrentThreadStorage();
    closeThreadHandle();
}

// used to wrap the incoming call from the platform-specific code
void SGP_API sgp_threadEntryPoint (void* userData)
{
    static_cast <Thread*> (userData)->threadEntryPoint();
}

//==============================================================================
void Thread::startThread()
{
    const ScopedLock sl (startStopLock);

    shouldExit = false;

    if (threadHandle == nullptr)
    {
        launchThread();
        setThreadPriority (threadHandle, threadPriority);
        startSuspensionEvent.signal();
    }
}

void Thread::startThread (const int priority)
{
    const ScopedLock sl (startStopLock);

    if (threadHandle == nullptr)
    {
        threadPriority = priority;
        startThread();
    }
    else
    {
        setPriority (priority);
    }
}

bool Thread::isThreadRunning() const
{
    return threadHandle != nullptr;
}

Thread* Thread::getCurrentThread()
{
    return getCurrentThreadHolder()->value.get();
}

//==============================================================================
void Thread::signalThreadShouldExit()
{
    shouldExit = true;
}

bool Thread::waitForThreadToExit (const int timeOutMilliseconds) const
{
    // Doh! So how exactly do you expect this thread to wait for itself to stop??
    jassert (getThreadId() != getCurrentThreadId() || getCurrentThreadId() == 0);

    const uint32 timeoutEnd = Time::getMillisecondCounter() + (uint32) timeOutMilliseconds;

    while (isThreadRunning())
    {
        if (timeOutMilliseconds >= 0 && Time::getMillisecondCounter() > timeoutEnd)
            return false;

        sleep (2);
    }

    return true;
}

void Thread::stopThread (const int timeOutMilliseconds)
{
    // agh! You can't stop the thread that's calling this method! How on earth
    // would that work??
    jassert (getCurrentThreadId() != getThreadId());

    const ScopedLock sl (startStopLock);

    if (isThreadRunning())
    {
        signalThreadShouldExit();
        notify();

        if (timeOutMilliseconds != 0)
            waitForThreadToExit (timeOutMilliseconds);

        if (isThreadRunning())
        {
            // very bad karma if this point is reached, as there are bound to be
            // locks and events left in silly states when a thread is killed by force..
            jassertfalse;
            //Logger::writeToLog ("!! killing thread by force !!");

            killThread();

            threadHandle = nullptr;
            threadId = 0;
        }
    }
}

//==============================================================================
bool Thread::setPriority (const int newPriority)
{
    // NB: deadlock possible if you try to set the thread prio from the thread itself,
    // so using setCurrentThreadPriority instead in that case.
    if (getCurrentThreadId() == getThreadId())
        return setCurrentThreadPriority (newPriority);

    const ScopedLock sl (startStopLock);

    if (setThreadPriority (threadHandle, newPriority))
    {
        threadPriority = newPriority;
        return true;
    }

    return false;
}

bool Thread::setCurrentThreadPriority (const int newPriority)
{
    return setThreadPriority (0, newPriority);
}

void Thread::setAffinityMask (const uint32 newAffinityMask)
{
    affinityMask = newAffinityMask;
}

//==============================================================================
bool Thread::wait (const int timeOutMilliseconds) const
{
    return defaultEvent.wait (timeOutMilliseconds);
}

void Thread::notify() const
{
    defaultEvent.signal();
}

//==============================================================================
void SpinLock::enter() const noexcept
{
    if (! tryEnter())
    {
        for (int i = 20; --i >= 0;)
            if (tryEnter())
                return;

        while (! tryEnter())
            Thread::yield();
    }
}


