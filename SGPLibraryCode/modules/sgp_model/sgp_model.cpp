// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_model.h"

namespace sgp
{
#include "model/sgp_modelFileMesh.cpp"
#include "model/sgp_modelFileBone.cpp"

}