// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_math.h"

//==============================================================================
namespace sgp
{
#include "math/sgp_Vector.cpp"
#include "math/sgp_Matrix4x4.cpp"
#include "math/sgp_Ray.cpp"
#include "math/sgp_AABBox.cpp"
#include "math/sgp_OBBox.cpp"
#include "math/sgp_Polygon.cpp"
#include "math/sgp_Plane.cpp"
#include "math/sgp_Quaternion.cpp"
#include "math/sgp_Frustum.cpp"
#include "math/sgp_Uuid.cpp"
#include "math/sgp_CollisionSet.cpp"
}
