#ifndef __SGP_ENGINEDEVICE_HEADER__
#define __SGP_ENGINEDEVICE_HEADER__

#include "../sgp_core/sgp_core.h"
#include "../sgp_math/sgp_math.h"
#include "../sgp_model/sgp_model.h"
#include "../sgp_particle/sgp_particle.h"
#include "../sgp_world/sgp_world.h"
#include "../sgp_render/sgp_render.h"

namespace sgp
{
#ifndef __SPG_EVENT_HEADER__
 #include "enginedevice/sgp_event.h"
#endif
#ifndef	__SGP_CREATIONPARAMETERS_HEADER__
 #include "enginedevice/sgp_CreationParameter.h"
#endif
#ifndef __SGP_VIDEOMODELIST_HEADER__
 #include "enginedevice/sgp_VideoModeList.h"
#endif
#ifndef __SPG_ENGINETIMER_HEADER__
 #include "enginedevice/sgp_EngineTimer.h"
#endif
#ifndef __SPG_ENGINESOUNDMANAGER_HEADER__
 #include "enginedevice/sgp_SoundManager.h"
#endif
#ifndef	__SGP_DEVICE_HEADER__
 #include "enginedevice/sgp_device.h"
#endif

#if SGP_WINDOWS
 #include "native/sgp_Device_win32.h"
#endif
}

#endif		// __SGP_ENGINEDEVICE_HEADER__