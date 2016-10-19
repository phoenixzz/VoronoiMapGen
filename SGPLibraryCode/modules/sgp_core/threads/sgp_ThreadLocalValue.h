

#ifndef __SGP_THREADLOCALVALUE_HEADER__
#define __SGP_THREADLOCALVALUE_HEADER__

// (NB: on win32, native thread-locals aren't possible in a dynamically loaded DLL in XP).
#if ! ((SGP_MSVC && (SGP_64BIT)) \
       || (SGP_MAC && SGP_CLANG && defined (MAC_OS_X_VERSION_10_7) \
             && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7))
 #define SGP_NO_COMPILER_THREAD_LOCAL 1
#endif

//==============================================================================
/**
    Provides cross-platform support for thread-local objects.

    This class holds an internal list of objects of the templated type, keeping
    an instance for each thread that requests one. The first time a thread attempts
    to access its value, an object is created and added to the list for that thread.

    Typically, you'll probably want to create a static instance of a ThreadLocalValue
    object, or hold one within a singleton.

    The templated class for your value could be a primitive type, or any class that
    has a default constructor and copy operator.

    When a thread no longer needs to use its value, it can call releaseCurrentThreadStorage()
    to allow the storage to be re-used by another thread. If a thread exits without calling
    this method, the object storage will be left allocated until the ThreadLocalValue object
    is deleted.
*/
template <typename Type>
class ThreadLocalValue
{
public:
    /** */
    ThreadLocalValue() noexcept
    {
    }

    /** Destructor.
        When this object is deleted, all the value objects for all threads will be deleted.
    */
    ~ThreadLocalValue()
    {
       #if SGP_NO_COMPILER_THREAD_LOCAL
        for (ObjectHolder* o = first.value; o != nullptr;)
        {
            ObjectHolder* const next = o->next;
            delete o;
            o = next;
        }
       #endif
    }

    /** Returns a reference to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
    */
    Type& operator*() const noexcept                        { return get(); }

    /** Returns a pointer to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
    */
    operator Type*() const noexcept                         { return &get(); }

    /** Accesses a method or field of the value object.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
    */
    Type* operator->() const noexcept                       { return &get(); }

    /** Assigns a new value to the thread-local object. */
    ThreadLocalValue& operator= (const Type& newValue)      { get() = newValue; return *this; }

    /** Returns a reference to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
    */
    Type& get() const noexcept
    {
       #if SGP_NO_COMPILER_THREAD_LOCAL
        const Thread::ThreadID threadId = Thread::getCurrentThreadId();

        for (ObjectHolder* o = first.get(); o != nullptr; o = o->next)
            if (o->threadId == threadId)
                return o->object;

        for (ObjectHolder* o = first.get(); o != nullptr; o = o->next)
        {
            if (o->threadId == nullptr)
            {
                {
                    SpinLock::ScopedLockType sl (lock);

                    if (o->threadId != nullptr)
                        continue;

                    o->threadId = threadId;
                }

                o->object = Type();
                return o->object;
            }
        }

        ObjectHolder* const newObject = new ObjectHolder (threadId);

        do
        {
            newObject->next = first.get();
        }
        while (! first.compareAndSetBool (newObject, newObject->next));

        return newObject->object;
       #elif SGP_MAC
        static __thread Type object;
        return object;
       #elif SGP_MSVC
        static __declspec(thread) Type object;
        return object;
       #endif
    }

    /** Called by a thread before it terminates, to allow this class to release
        any storage associated with the thread.
    */
    void releaseCurrentThreadStorage()
    {
       #if SGP_NO_COMPILER_THREAD_LOCAL
        const Thread::ThreadID threadId = Thread::getCurrentThreadId();

        for (ObjectHolder* o = first.get(); o != nullptr; o = o->next)
        {
            if (o->threadId == threadId)
            {
                SpinLock::ScopedLockType sl (lock);
                o->threadId = nullptr;
            }
        }
       #endif
    }

private:
    //==============================================================================
   #if SGP_NO_COMPILER_THREAD_LOCAL
    struct ObjectHolder
    {
        ObjectHolder (const Thread::ThreadID& tid)
            : threadId (tid), object()
        {}

        Thread::ThreadID threadId;
        ObjectHolder* next;
        Type object;

        SGP_DECLARE_NON_COPYABLE (ObjectHolder)
    };

    mutable Atomic<ObjectHolder*> first;
    SpinLock lock;
   #endif

    SGP_DECLARE_NON_COPYABLE (ThreadLocalValue)
};


#endif   // __SGP_THREADLOCALVALUE_HEADER__
