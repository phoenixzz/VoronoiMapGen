

#ifndef __SGP_SINGLETON_HEADER__
#define __SGP_SINGLETON_HEADER__


//==============================================================================
/**
    Macro to declare member variables and methods for a singleton class.

    To use this, add the line sgp_DeclareSingleton_SingleThreaded (MyClass)
    to the class's definition.

    Then put a macro sgp_ImplementSingleton_SingleThreaded (MyClass) along with the class's
    implementation code.

    It's also a very good idea to also add the call clearSingletonInstance() in your class's
    destructor, in case it is deleted by other means than deleteInstance()

    Clients can then call the static method MyClass::getInstance() to get a pointer
    to the singleton, or MyClass::getInstanceWithoutCreating() which will return 0 if
    no instance currently exists.

    e.g. @code

        class MySingleton
        {
        public:
            MySingleton()
            {
            }

            ~MySingleton()
            {
                // this ensures that no dangling pointers are left when the
                // singleton is deleted.
                clearSingletonInstance();
            }

            sgp_DeclareSingleton_SingleThreaded (MySingleton)
        };

        sgp_ImplementSingleton_SingleThreaded (MySingleton)


        // example of usage:
        MySingleton* m = MySingleton::getInstance(); // creates the singleton if there isn't already one.

        ...

        MySingleton::deleteInstance(); // safely deletes the singleton (if it's been created).

    @endcode




*/
#if 0

#define juce_DeclareSingleton(classname, doNotRecreateAfterDeletion) \
\
    static classname* _singletonInstance;  \
    static juce::CriticalSection _singletonLock; \
\
    static classname* JUCE_CALLTYPE getInstance() \
    { \
        if (_singletonInstance == nullptr) \
        {\
            const juce::ScopedLock sl (_singletonLock); \
\
            if (_singletonInstance == nullptr) \
            { \
                static bool alreadyInside = false; \
                static bool createdOnceAlready = false; \
\
                const bool problem = alreadyInside || ((doNotRecreateAfterDeletion) && createdOnceAlready); \
                jassert (! problem); \
                if (! problem) \
                { \
                    createdOnceAlready = true; \
                    alreadyInside = true; \
                    classname* newObject = new classname();  /* (use a stack variable to avoid setting the newObject value before the class has finished its constructor) */ \
                    alreadyInside = false; \
\
                    _singletonInstance = newObject; \
                } \
            } \
        } \
\
        return _singletonInstance; \
    } \
\
    static inline classname* JUCE_CALLTYPE getInstanceWithoutCreating() noexcept\
    { \
        return _singletonInstance; \
    } \
\
    static void JUCE_CALLTYPE deleteInstance() \
    { \
        const juce::ScopedLock sl (_singletonLock); \
        if (_singletonInstance != nullptr) \
        { \
            classname* const old = _singletonInstance; \
            _singletonInstance = nullptr; \
            delete old; \
        } \
    } \
\
    void clearSingletonInstance() noexcept\
    { \
        if (_singletonInstance == this) \
            _singletonInstance = nullptr; \
    }


//==============================================================================
/** This is a counterpart to the juce_DeclareSingleton macro.

    After adding the juce_DeclareSingleton to the class definition, this macro has
    to be used in the cpp file.
*/
#define juce_ImplementSingleton(classname) \
\
    classname* classname::_singletonInstance = nullptr; \
    juce::CriticalSection classname::_singletonLock;


//==============================================================================
/**
    Macro to declare member variables and methods for a singleton class.

    This is exactly the same as juce_DeclareSingleton, but doesn't use a critical
    section to make access to it thread-safe. If you know that your object will
    only ever be created or deleted by a single thread, then this is a
    more efficient version to use.

    If doNotRecreateAfterDeletion = true, it won't allow the object to be created more
    than once during the process's lifetime - i.e. after you've created and deleted the
    object, getInstance() will refuse to create another one. This can be useful to stop
    objects being accidentally re-created during your app's shutdown code.

    See the documentation for juce_DeclareSingleton for more information about
    how to use it, the only difference being that you have to use
    juce_ImplementSingleton_SingleThreaded instead of juce_ImplementSingleton.

    @see juce_ImplementSingleton_SingleThreaded, juce_DeclareSingleton, juce_DeclareSingleton_SingleThreaded_Minimal
*/
#define juce_DeclareSingleton_SingleThreaded(classname, doNotRecreateAfterDeletion) \
\
    static classname* _singletonInstance;  \
\
    static classname* getInstance() \
    { \
        if (_singletonInstance == nullptr) \
        { \
            static bool alreadyInside = false; \
            static bool createdOnceAlready = false; \
\
            const bool problem = alreadyInside || ((doNotRecreateAfterDeletion) && createdOnceAlready); \
            jassert (! problem); \
            if (! problem) \
            { \
                createdOnceAlready = true; \
                alreadyInside = true; \
                classname* newObject = new classname();  /* (use a stack variable to avoid setting the newObject value before the class has finished its constructor) */ \
                alreadyInside = false; \
\
                _singletonInstance = newObject; \
            } \
        } \
\
        return _singletonInstance; \
    } \
\
    static inline classname* getInstanceWithoutCreating() noexcept\
    { \
        return _singletonInstance; \
    } \
\
    static void deleteInstance() \
    { \
        if (_singletonInstance != nullptr) \
        { \
            classname* const old = _singletonInstance; \
            _singletonInstance = nullptr; \
            delete old; \
        } \
    } \
\
    void clearSingletonInstance() noexcept\
    { \
        if (_singletonInstance == this) \
            _singletonInstance = nullptr; \
    }


#endif
//==============================================================================
/**
    Macro to declare member variables and methods for a singleton class.

    This doesn't do any checking
    for recursion or repeated instantiation. It's intended for use as a lightweight
    version of a singleton, where you're using it in very straightforward
    circumstances and don't need the extra checking.
*/
#define sgp_DeclareSingleton_SingleThreaded(classname) \
\
    static classname* _singletonInstance;  \
\
    static classname* getInstance() \
    { \
        if (_singletonInstance == nullptr) \
            _singletonInstance = new classname(); \
\
        return _singletonInstance; \
    } \
\
    static inline classname* getInstanceWithoutCreating() noexcept\
    { \
        return _singletonInstance; \
    } \
\
    static void deleteInstance() \
    { \
        if (_singletonInstance != nullptr) \
        { \
            classname* const old = _singletonInstance; \
            _singletonInstance = nullptr; \
            delete old; \
        } \
    } \
\
    void clearSingletonInstance() noexcept\
    { \
        if (_singletonInstance == this) \
            _singletonInstance = nullptr; \
    }


//==============================================================================
/** This is a counterpart to the sgp_DeclareSingleton_SingleThreaded macro.

    After adding sgp_DeclareSingleton_SingleThreaded
    to the class definition, this macro has to be used somewhere in the cpp file.
*/
#define sgp_ImplementSingleton_SingleThreaded(classname) \
\
    classname* classname::_singletonInstance = nullptr;



#endif   // __SGP_SINGLETON_HEADER__
