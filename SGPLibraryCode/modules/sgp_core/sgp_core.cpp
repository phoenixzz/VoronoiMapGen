// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "native/sgp_BasicNativeHeaders.h"
#include "sgp_core.h"

#include <locale>
#include <cctype>
#include <sys/timeb.h>

#if ! SGP_ANDROID
 #include <cwctype>
#endif

#if SGP_WINDOWS
 #include <ctime>
 #include <winsock2.h>
 #include <ws2tcpip.h>


#else
 #if SGP_LINUX || SGP_ANDROID
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/errno.h>
  #include <unistd.h>
  #include <netinet/in.h>
 #endif

 #if SGP_LINUX
  #include <langinfo.h>
 #endif

 #include <pwd.h>
 #include <fcntl.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
 #include <sys/time.h>
#endif

#if SGP_MAC || SGP_IOS
 #include <xlocale.h>
#endif

#if SGP_ANDROID
 #include <android/log.h>
#endif


//==============================================================================
namespace sgp
{

/*
#include "containers/juce_AbstractFifo.cpp"
#include "containers/juce_DynamicObject.cpp"
#include "containers/juce_NamedValueSet.cpp"
#include "containers/juce_PropertySet.cpp"
#include "containers/juce_Variant.cpp"
*/
#include "files/sgp_DirectoryIterator.cpp"
#include "files/sgp_File.cpp"
#include "files/sgp_FileInputStream.cpp"
#include "files/sgp_FileOutputStream.cpp"
/*
#include "files/juce_FileSearchPath.cpp"
#include "files/juce_TemporaryFile.cpp"
#include "json/juce_JSON.cpp"
*/
#include "log/sgp_FileLogger.cpp"
#include "log/sgp_Logger.cpp"
#include "log/sgp_ConsoleLogger.cpp"
/*
#include "maths/juce_BigInteger.cpp"
#include "maths/juce_Expression.cpp"
#include "maths/juce_Random.cpp"
*/
#include "common/sgp_MemoryBlock.cpp"
#include "common/sgp_Result.cpp"
#include "common/sgp_Colour.cpp"
#include "common/sgp_Random.cpp"
/*
#include "streams/juce_BufferedInputStream.cpp"
#include "streams/juce_FileInputSource.cpp"
*/
#include "streams/sgp_InputStream.cpp"
#include "streams/sgp_MemoryInputStream.cpp"
#include "streams/sgp_MemoryOutputStream.cpp"
#include "streams/sgp_OutputStream.cpp"
#include "streams/sgp_FileInputSource.cpp"

/*
#include "streams/juce_SubregionStream.cpp"
*/
#include "system/sgp_SystemStats.cpp"
#include "text/sgp_CharacterFunctions.cpp"
/*
#include "text/juce_Identifier.cpp"
#include "text/juce_LocalisedStrings.cpp"
*/
#include "text/sgp_String.cpp"
#include "text/sgp_StringArray.cpp"
#include "text/sgp_StringPool.cpp"
/*
#include "text/juce_StringPairArray.cpp"


#include "text/juce_TextDiff.cpp"
#include "threads/juce_ChildProcess.cpp"
#include "threads/juce_ReadWriteLock.cpp"
*/
#include "threads/sgp_Thread.cpp"
/*
#include "threads/juce_ThreadPool.cpp"
#include "threads/juce_TimeSliceThread.cpp"
#include "time/juce_PerformanceCounter.cpp"
*/
#include "time/sgp_RelativeTime.cpp"
#include "time/sgp_Time.cpp"

#include "xml/sgp_XmlElement.cpp"
#include "xml/sgp_XmlDocument.cpp"
/*
#include "unit_tests/juce_UnitTest.cpp"
#include "xml/juce_XmlDocument.cpp"
#include "xml/juce_XmlElement.cpp"
#include "zip/juce_GZIPDecompressorInputStream.cpp"
#include "zip/juce_GZIPCompressorOutputStream.cpp"
#include "zip/juce_ZipFile.cpp"
*/

//==============================================================================
#if SGP_MAC || SGP_IOS
#include "native/sgp_osx_ObjCHelpers.h"
#endif

#if SGP_ANDROID
#include "native/sgp_android_JNIHelpers.h"
#endif

#if ! SGP_WINDOWS
#include "native/sgp_posix_SharedCode.h"
#include "native/sgp_posix_NamedPipe.cpp"
#endif

//==============================================================================
#if SGP_MAC || SGP_IOS
#include "native/sgp_mac_Files.mm"
//#include "native/juce_mac_Network.mm"
#include "native/sgp_mac_Strings.mm"
#include "native/sgp_mac_SystemStats.mm"
#include "native/sgp_mac_Threads.mm"

//==============================================================================
#elif SGP_WINDOWS
//#include "native/juce_win32_ComSmartPtr.h"
#include "native/sgp_win32_Files.cpp"
//#include "native/juce_win32_Network.cpp"
//#include "native/juce_win32_Registry.cpp"
#include "native/sgp_win32_SystemStats.cpp"
#include "native/sgp_win32_Threads.cpp"

//==============================================================================
#elif SGP_LINUX
#include "native/sgp_linux_Files.cpp"
//#include "native/juce_linux_Network.cpp"
#include "native/sgp_linux_SystemStats.cpp"
#include "native/sgp_linux_Threads.cpp"

//==============================================================================
#elif SGP_ANDROID
#include "native/sgp_android_Files.cpp"
//#include "native/juce_android_Misc.cpp"
//#include "native/juce_android_Network.cpp"
#include "native/sgp_android_SystemStats.cpp"
#include "native/sgp_android_Threads.cpp"

#endif

}
