#ifndef __SGP_PARTICLE_HEADER__
#define __SGP_PARTICLE_HEADER__

#include "../sgp_core/sgp_core.h"
#include "../sgp_math/sgp_math.h"

namespace sgp
{
	namespace SPARK
	{
	#ifndef __SGP_SPARKDEFINE_HEADER__
	 #include "core/sgp_SPARK_Define.h"
	#endif
	#ifndef __SGP_SPARKBUFFER_HEADER__
	 #include "core/sgp_SPARK_Buffer.h"
	#endif
	#ifndef __SGP_SPARKARRAYBUFFER_HEADER__
	 #include "core/sgp_SPARK_ArrayBuffer.h"
	#endif
	#ifndef __SGP_SPARKREGISTERABLE_HEADER__
	 #include "core/sgp_SPARK_Registerable.h"
	#endif
	#ifndef __SGP_SPARKTRANSFORMABLE_HEADER__
	 #include "core/sgp_SPARK_Transformable.h"
	#endif
	#ifndef __SGP_SPARKBUFFERHANDLER_HEADER__
	 #include "core/sgp_SPARK_BufferHandler.h"
	#endif
	#ifndef __SGP_SPARKREGWRAPPER_HEADER__
	 #include "core/sgp_SPARK_RegWrapper.h"
	#endif
	#ifndef __SGP_SPARKRENDERER_HEADER__
	 #include "core/sgp_SPARK_Renderer.h"
	#endif
	#ifndef __SGP_SPARKSYSTEM_HEADER__
	 #include "core/sgp_SPARK_System.h"
	#endif
	#ifndef __SGP_SPARKINTERPOLATOR_HEADER__
	 #include "core/sgp_SPARK_Interpolator.h"
	#endif
	#ifndef __SGP_SPARKPOOL_HEADER__
	 #include "core/sgp_SPARK_Pool.h"
	#endif
	#ifndef __SGP_SPARKZONE_HEADER__
	 #include "core/sgp_SPARK_Zone.h"
	#endif
	#ifndef __SGP_SPARKMODEL_HEADER__
	 #include "core/sgp_SPARK_Model.h"
	#endif
	#ifndef __SGP_SPARKPARTICLE_HEADER__
	 #include "core/sgp_SPARK_Particle.h"
	#endif
	#ifndef __SGP_SPARKEMITTER_HEADER__
	 #include "core/sgp_SPARK_Emitter.h"
	#endif
	#ifndef __SGP_SPARKMODIFIER_HEADER__
	 #include "core/sgp_SPARK_Modifier.h"
	#endif
	#ifndef __SGP_SPARKGROUP_HEADER__
	 #include "core/sgp_SPARK_Group.h"
	#endif
	#ifndef __SGP_SPARKFACTORY_HEADER__
	 #include "core/sgp_SPARK_Factory.h"
	#endif


	#ifndef __SGP_SPARKAABBOXZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_AABBoxZone.h"
	#endif
	#ifndef __SGP_SPARKPOINTZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_PointZone.h"
	#endif
	#ifndef __SGP_SPARKSPHEREZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_SphereZone.h"
	#endif
	#ifndef __SGP_SPARKPLANEZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_PlaneZone.h"
	#endif
	#ifndef __SGP_SPARKRINGZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_RingZone.h"
	#endif
	#ifndef __SGP_SPARKLINEZONE_HEADER__
	 #include "extension/zones/sgp_SPARK_LineZone.h"
	#endif


	#ifndef __SGP_SPARKRANDOMEMITTER_HEADER__
	 #include "extension/emitters/sgp_SPARK_RandomEmitter.h"
	#endif
	#ifndef __SGP_SPARKSTATICEMITTER_HEADER__
	 #include "extension/emitters/sgp_SPARK_StaticEmitter.h"
	#endif
	#ifndef __SGP_SPARKSTRAIGHTEMITTER_HEADER__
	 #include "extension/emitters/sgp_SPARK_StraightEmitter.h"
	#endif
	#ifndef __SGP_SPARKSPHERICEMITTER_HEADER__
	 #include "extension/emitters/sgp_SPARK_SphericEmitter.h"
	#endif
	#ifndef __SGP_SPARKNORMALEMITTER_HEADER__
	 #include "extension/emitters/sgp_SPARK_NormalEmitter.h"
	#endif


	#ifndef __SGP_SPARKOBSTACLE_HEADER__
	 #include "extension/modifiers/sgp_SPARK_Obstacle.h"
	#endif
	#ifndef __SGP_SPARKLINEARFORCE_HEADER__
	 #include "extension/modifiers/sgp_SPARK_LinearForce.h"
	#endif
	#ifndef __SGP_SPARKPOINTMASS_HEADER__
	 #include "extension/modifiers/sgp_SPARK_PointMass.h"
	#endif


	#ifndef __SGP_SPARKPOINTRENDERINTERFACE_HEADER__
	 #include "extension/renderers/sgp_SPARK_PointRendererInterface.h"
	#endif
	#ifndef __SGP_SPARKQUADRENDERINTERFACE_HEADER__
	 #include "extension/renderers/sgp_SPARK_QuadRendererInterface.h"
	#endif
	#ifndef __SGP_SPARKLINERENDERINTERFACE_HEADER__
	 #include "extension/renderers/sgp_SPARK_LineRendererInterface.h"
	#endif
	#ifndef __SGP_SPARKORIENTED2DRENDERINTERFACE_HEADER__
	 #include "extension/renderers/sgp_SPARK_Oriented2DRendererInterface.h"
	#endif
	#ifndef __SGP_SPARKORIENTED3DRENDERINTERFACE_HEADER__
	 #include "extension/renderers/sgp_SPARK_Oriented3DRendererInterface.h"
	#endif
	}
}

#endif		// __SGP_PARTICLE_HEADER__