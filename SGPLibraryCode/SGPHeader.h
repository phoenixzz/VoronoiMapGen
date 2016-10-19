

#ifndef __SGPHEADERFILE_HEADER__
#define __SGPHEADERFILE_HEADER__

#include "AppConfig.h"
#include "modules/sgp_core/sgp_core.h"
#include "modules/sgp_math/sgp_math.h"
#include "modules/sgp_model/sgp_model.h"
#include "modules/sgp_particle/sgp_particle.h"
#include "modules/sgp_world/sgp_world.h"
#include "modules/sgp_render/sgp_render.h"
#include "modules/sgp_enginedevice/sgp_enginedevice.h"

using namespace sgp;


namespace ProjectInfo
{
    const char* const  projectName    = "SGPEngine";
    const char* const  versionString  = "1.0.0";
    const int          versionNumber  = 0x10000;
}

#endif   // __SGPHEADERFILE_HEADER__
