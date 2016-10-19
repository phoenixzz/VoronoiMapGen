// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_enginedevice.h"

//==============================================================================
namespace sgp
{
#include "enginedevice/sgp_VideoModeList.cpp"
}

#if SGP_WINDOWS
	#include "native/sgp_Device_win32.cpp"
#endif

namespace sgp
{
#include "enginedevice/sgp_device.cpp"
}