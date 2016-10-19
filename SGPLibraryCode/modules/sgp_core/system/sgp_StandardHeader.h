
#ifndef __SGP_STANDARDHEADER_HEADER__
#define __SGP_STANDARDHEADER_HEADER__

//==============================================================================
/** Current SGP Engine version number.

    See also SystemStats::getSGPVersion() for a string version.
*/
#define SGP_MAJOR_VERSION      1
#define SGP_MINOR_VERSION      0
#define SGP_BUILDNUMBER        0

/** Current sgp version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also SystemStats::getSGPVersion() for a string version.
*/
#define SGP_VERSION   ((SGP_MAJOR_VERSION << 16) + (SGP_MINOR_VERSION << 8) + SGP_BUILDNUMBER)


//==============================================================================
#include "sgp_TargetPlatform.h"  // (sets up the various SGP_WINDOWS, SGP_MAC, etc flags)
#include "sgp_PlatformDefs.h"

//==============================================================================
// Now we'll include some common OS headers..
#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4514 4245 4100)
#endif

#include <cstdlib>
#include <cstdarg>
#include <climits>
#include <limits>
#include <cmath>
#include <cwchar>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <new>



#if SGP_USE_INTRINSICS
 #include <intrin.h>
#endif

#if SGP_MAC || SGP_IOS
 #include <libkern/OSAtomic.h>
#endif

#if SGP_LINUX
 #include <signal.h>

 #if __INTEL_COMPILER
  #if __ia64__
   #include <ia64intrin.h>
  #else
   #include <ia32intrin.h>
  #endif
 #endif
#endif

#if SGP_MSVC && SGP_DEBUG
 #include <crtdbg.h>
#endif

#if SGP_MSVC
 #pragma warning (pop)
#endif

#if SGP_ANDROID
 #include <sys/atomics.h>
 #include <byteswap.h>
#endif

// undef symbols that are sometimes set by misguided 3rd-party headers..
#undef check
#undef TYPE_BOOL
#undef max
#undef min

#if SGP_MSVC
	#define STRNICMP _strnicmp
#elif SGP_MAC || SGP_IOS
	#define STRNICMP strncasecmp
#elif SGP_ANDROID
	#define STRNICMP _strnicmp
#endif


//==============================================================================
#ifndef SGP_API
 #define SGP_API   /**< This macro is added to all sgp public class declarations. */
#endif

/** This macro is added to all sgp public function declarations. */
#define SGP_PUBLIC_FUNCTION        SGP_API SGP_CALLTYPE

//==============================================================================
// Now include some common headers...
namespace sgp
{
    extern SGP_API bool SGP_CALLTYPE sgp_isRunningUnderDebugger();

}

#endif   // __SGP_STANDARDHEADER_HEADER__
