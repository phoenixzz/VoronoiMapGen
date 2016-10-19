#ifndef __SGP_TARGETPLATFORM_HEADER__
#define __SGP_TARGETPLATFORM_HEADER__

//==============================================================================
/*  This file figures out which platform is being built, and defines some macros
    that the rest of the code can use for OS-specific compilation.

    Macros that will be set here are:

    - One of SGP_WINDOWS, SGP_MAC SGP_LINUX, SGP_IOS, SGP_ANDROID, etc.
    - Either SGP_32BIT or SGP_64BIT, depending on the architecture.
    - Either SGP_LITTLE_ENDIAN or SGP_BIG_ENDIAN.
    - Either SGP_INTEL or SGP_PPC
    - Either SGP_GCC or SGP_MSVC
*/

//==============================================================================
#if (defined (_WIN32) || defined (_WIN64))
  #define       SGP_WIN32 1
  #define       SGP_WINDOWS 1
#elif defined (SGP_ANDROID)
  #undef        SGP_ANDROID
  #define       SGP_ANDROID 1
#elif defined (LINUX) || defined (__linux__)
  #define     SGP_LINUX 1
#elif defined (__APPLE_CPP__) || defined(__APPLE_CC__)
  #define Point CarbonDummyPointName // (workaround to avoid definition of "Point" by old Carbon headers)
  #define Component CarbonDummyCompName
  #include <CoreFoundation/CoreFoundation.h> // (needed to find out what platform we're using)
  #undef Point
  #undef Component

  #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #define     SGP_IPHONE 1
    #define     SGP_IOS 1
  #else
    #define     SGP_MAC 1
  #endif
#else
  #error "Unknown platform!"
#endif

//==============================================================================
#if SGP_WINDOWS
  #ifdef _MSC_VER
    #ifdef _WIN64
      #define SGP_64BIT 1
    #else
      #define SGP_32BIT 1
    #endif
  #endif

  #ifdef _DEBUG
    #define SGP_DEBUG 1
  #endif


  /** If defined, this indicates that the processor is little-endian. */
  #define SGP_LITTLE_ENDIAN 1

  #define SGP_INTEL 1
#endif

//==============================================================================
#if SGP_MAC || SGP_IOS

  #if defined (DEBUG) || defined (_DEBUG) || ! (defined (NDEBUG) || defined (_NDEBUG))
    #define SGP_DEBUG 1
  #endif

  #if ! (defined (DEBUG) || defined (_DEBUG) || defined (NDEBUG) || defined (_NDEBUG))
    #warning "Neither NDEBUG or DEBUG has been defined - you should set one of these to make it clear whether this is a release build,"
  #endif

  #ifdef __LITTLE_ENDIAN__
    #define SGP_LITTLE_ENDIAN 1
  #else
    #define SGP_BIG_ENDIAN 1
  #endif
#endif

#if SGP_MAC

  #if defined (__ppc__) || defined (__ppc64__)
    #define SGP_PPC 1
  #else
    #define SGP_INTEL 1
  #endif

  #ifdef __LP64__
    #define SGP_64BIT 1
  #else
    #define SGP_32BIT 1
  #endif

  #if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
    #error "Building for OSX 10.3 is no longer supported!"
  #endif

  #ifndef MAC_OS_X_VERSION_10_5
    #error "To build with 10.4 compatibility, use a 10.5 or 10.6 SDK and set the deployment target to 10.4"
  #endif

#endif

//==============================================================================
#if SGP_LINUX || SGP_ANDROID

  #ifdef _DEBUG
    #define SGP_DEBUG 1
  #endif

  // Allow override for big-endian Linux platforms
  #if defined (__LITTLE_ENDIAN__) || ! defined (SGP_BIG_ENDIAN)
    #define SGP_LITTLE_ENDIAN 1
    #undef SGP_BIG_ENDIAN
  #else
    #undef SGP_LITTLE_ENDIAN
    #define SGP_BIG_ENDIAN 1
  #endif

  #if defined (__LP64__) || defined (_LP64)
    #define SGP_64BIT 1
  #else
    #define SGP_32BIT 1
  #endif

  #if __MMX__ || __SSE__ || __amd64__
    #define SGP_INTEL 1
  #endif
#endif

//==============================================================================
// Compiler type macros.

#ifdef __clang__
 #define SGP_CLANG 1
 #define SGP_GCC 1
#elif defined (__GNUC__)
  #define SGP_GCC 1
#elif defined (_MSC_VER)
  #define SGP_MSVC 1

  #if _MSC_VER < 1500
    #define SGP_VC8_OR_EARLIER 1

    #if _MSC_VER < 1400
      #define SGP_VC7_OR_EARLIER 1

      #if _MSC_VER < 1300
        #warning "MSVC 6.0 is no longer supported!"
      #endif
    #endif
  #endif

  #if SGP_64BIT || ! SGP_VC7_OR_EARLIER
    #define SGP_USE_INTRINSICS 1
  #endif
#else
  #error unknown compiler
#endif

#endif   // __SGP_TARGETPLATFORM_HEADER__
