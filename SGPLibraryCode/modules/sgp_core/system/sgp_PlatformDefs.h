#ifndef __SGP_PLATFORMDEFS_HEADER__
#define __SGP_PLATFORMDEFS_HEADER__

//==============================================================================
/*  This file defines miscellaneous macros for debugging, assertions, etc.
*/

//==============================================================================


/** This macro defines the C calling convention used as the standard for sgp calls. */
#if SGP_MSVC
 #define SGP_CALLTYPE   __stdcall
 #define SGP_CDECL      __cdecl
#else
 #define SGP_CALLTYPE
 #define SGP_CDECL
#endif

#if SGP_DEBUG
 #define sgp_LogCurrentAssertion    std::cerr << "SGP Engine Assertion failure in " << __FILE__ << ", line " << __LINE__ << std::endl;
#else
 #define sgp_LogCurrentAssertion
#endif

//==============================================================================
#if SGP_IOS || SGP_LINUX || SGP_ANDROID || SGP_PPC
  /** This will try to break into the debugger if the app is currently being debugged.
      If called by an app that's not being debugged, the behaiour isn't defined - it may crash or not, depending
      on the platform.
  */
  #define sgp_breakDebugger        { ::kill (0, SIGTRAP); }
#elif SGP_USE_INTRINSICS
  #ifndef __INTEL_COMPILER
    #pragma intrinsic (__debugbreak)
  #endif
  #define sgp_breakDebugger        { __debugbreak(); }
#elif SGP_GCC || SGP_MAC
  #if SGP_NO_INLINE_ASM
   #define sgp_breakDebugger       { }
  #else
   #define sgp_breakDebugger       { asm ("int $3"); }
  #endif
#else
  #define sgp_breakDebugger        { __asm int 3 }
#endif


//==============================================================================
#if SGP_DEBUG
  /** Writes a string to the standard error stream.
      This is only compiled in a debug build.
  */

  #define DBG(dbgtext)              { sgp::String tempDbgBuf; tempDbgBuf << dbgtext; sgp::Logger::outputDebugString (tempDbgBuf); }

  //==============================================================================
  /** This will always cause an assertion failure.
      It is only compiled in a debug build.
  */
  #define jassertfalse              { sgp_LogCurrentAssertion; if (sgp::sgp_isRunningUnderDebugger()) sgp_breakDebugger; }

  //==============================================================================
  /** Platform-independent assertion macro.

      This macro gets turned into a no-op when you're building with debugging turned off, so be
      careful that the expression you pass to it doesn't perform any actions that are vital for the
      correct behaviour of your program!
  */
  #define jassert(expression)       { if (! (expression)) jassertfalse; }

#else
  //==============================================================================
  // If debugging is disabled, these dummy debug and assertion macros are used..

  #define DBG(dbgtext)
  #define jassertfalse              { sgp_LogCurrentAssertion }

  #define jassert(a)                {}


#endif

//==============================================================================

namespace sgp
{
    template <bool b> struct SGPStaticAssert;
    template <> struct SGPStaticAssert <true> { static void dummy() {} };
}


/** A compile-time assertion macro.
    If the expression parameter is false, the macro will cause a compile error. (The actual error
    message that the compiler generates may be completely bizarre and seem to have no relation to
    the place where you put the static_assert though!)
*/
#define static_jassert(expression)      sgp::SGPStaticAssert<expression>::dummy();

/** This is a shorthand macro for declaring stubs for a class's copy constructor and operator=.

    For example, instead of
    @code
    class MyClass
    {
        etc..

    private:
        MyClass (const MyClass&);
        MyClass& operator= (const MyClass&);
    };@endcode

    ..you can just write:

    @code
    class MyClass
    {
        etc..

    private:
        SGP_DECLARE_NON_COPYABLE (MyClass)
    };@endcode
*/
#define SGP_DECLARE_NON_COPYABLE(className) \
    className (const className&);\
    className& operator= (const className&);

/** This is a shorthand way of writing both a SGP_DECLARE_NON_COPYABLE and
    SGP_LEAK_DETECTOR macro for a class.
*/
//#define SGP_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(className) \
//    SGP_DECLARE_NON_COPYABLE(className) \
//    SGP_LEAK_DETECTOR(className)

/** This macro can be added to class definitions to disable the use of new/delete to
    allocate the object on the heap, forcing it to only be used as a stack or member variable.
*/
#define SGP_PREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t); \
    static void operator delete (void*);


//==============================================================================

 #define SGP_JOIN_MACRO_HELPER(a, b) a ## b
 #define SGP_STRINGIFY_MACRO_HELPER(a) #a


/** A good old-fashioned C macro concatenation helper.
    This combines two items (which may themselves be macros) into a single string,
    avoiding the pitfalls of the ## macro operator.
*/
#define SGP_JOIN_MACRO(item1, item2)  SGP_JOIN_MACRO_HELPER (item1, item2)

/** A handy C macro for stringifying any symbol, rather than just a macro parameter.
*/
#define SGP_STRINGIFY(item)  SGP_STRINGIFY_MACRO_HELPER (item)




#define SGP_TRY
#define SGP_CATCH_EXCEPTION
#define SGP_CATCH_ALL
#define SGP_CATCH_ALL_ASSERT



//==============================================================================
#if SGP_DEBUG
  /** A platform-independent way of forcing an inline function.
      Use the syntax: @code
      forcedinline void myfunction (int x)
      @endcode
  */
  #define forcedinline  inline
#else
  #if SGP_MSVC
   #define forcedinline       __forceinline
  #else
   #define forcedinline       inline __attribute__((always_inline))
  #endif
#endif

#if SGP_MSVC
  /** This can be placed before a stack or member variable declaration to tell the compiler
      to align it to the specified number of bytes. */
  #define SGP_ALIGN(bytes)   __declspec (align (bytes))
#else
  #define SGP_ALIGN(bytes)   __attribute__ ((aligned (bytes)))
#endif

//==============================================================================
// Cross-compiler deprecation macros..
#if SGP_MSVC
 /** This can be used to wrap a function which has been deprecated. */
 #define SGP_DEPRECATED(functionDef)     __declspec(deprecated) functionDef
#elif SGP_GCC 
 #define SGP_DEPRECATED(functionDef)     functionDef __attribute__ ((deprecated))
#else
 #define SGP_DEPRECATED(functionDef)     functionDef
#endif


//==============================================================================
#if SGP_GCC
 #define SGP_PACKED __attribute__((packed))
#else
 #define SGP_PACKED
#endif


//==============================================================================
// Declare some fake versions of nullptr and noexcept, for older compilers:

 #ifdef noexcept
  #undef noexcept
 #endif
 #define noexcept  throw()
 #if defined (_MSC_VER) && _MSC_VER > 1600
  #define _ALLOW_KEYWORD_MACROS 1 // (to stop VC2012 complaining)
 #endif


 #ifdef nullptr
  #undef nullptr
 #endif
 #define nullptr (0)


#endif   // __SGP_PLATFORMDEFS_HEADER__
