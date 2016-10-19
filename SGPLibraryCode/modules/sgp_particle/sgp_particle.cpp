// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_particle.h"

namespace sgp
{
	namespace SPARK
	{
		#include "core/sgp_SPARK_Define.cpp"
		#include "core/sgp_SPARK_Registerable.cpp"
		#include "core/sgp_SPARK_Transformable.cpp"
		#include "core/sgp_SPARK_BufferHandler.cpp"
		#include "core/sgp_SPARK_System.cpp"
		#include "core/sgp_SPARK_Particle.cpp"
		#include "core/sgp_SPARK_Zone.cpp"
		#include "core/sgp_SPARK_Interpolator.cpp"
		#include "core/sgp_SPARK_Model.cpp"
		#include "core/sgp_SPARK_Emitter.cpp"
		#include "core/sgp_SPARK_Modifier.cpp"
		#include "core/sgp_SPARK_Group.cpp"
		#include "core/sgp_SPARK_Factory.cpp"

		#include "extension/zones/sgp_SPARK_AABBoxZone.cpp"
		#include "extension/zones/sgp_SPARK_SphereZone.cpp"
		#include "extension/zones/sgp_SPARK_PlaneZone.cpp"
		#include "extension/zones/sgp_SPARK_RingZone.cpp"
		#include "extension/zones/sgp_SPARK_LineZone.cpp"

		#include "extension/emitters/sgp_SPARK_RandomEmitter.cpp"
		#include "extension/emitters/sgp_SPARK_SphericEmitter.cpp"
		#include "extension/emitters/sgp_SPARK_NormalEmitter.cpp"

		#include "extension/modifiers/sgp_SPARK_Obstacle.cpp"
		#include "extension/modifiers/sgp_SPARK_LinearForce.cpp"
		#include "extension/modifiers/sgp_SPARK_PointMass.cpp"
	}
}