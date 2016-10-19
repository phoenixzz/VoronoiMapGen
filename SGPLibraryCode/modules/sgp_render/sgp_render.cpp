// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_render.h"

#if defined (BUILD_OGLES2)
	#include "opengles2/sgp_opengles2.cpp"
#else
	#include "opengl/sgp_opengl.cpp"
#endif

namespace sgp
{
#include "camera/sgp_MovementController.cpp"

#include "texturesystem/sgp_ColorConverter.cpp"
#include "texturesystem/sgp_ImageUncompressed.cpp"
#include "texturesystem/sgp_ImageDDS.cpp"

#if defined (BUILD_OGLES2)

#else
	#include "texturesystem/sgp_ImageLoaderTGA.cpp"
	#include "texturesystem/sgp_ImageLoaderDDS.cpp"
#endif

#include "texturesystem/sgp_TextureManager.cpp"

#include "materialsystem/sgp_AbstractMaterial.cpp"
#include "materialsystem/sgp_MaterialSystem.cpp"

#include "modelsystem/sgp_ModelManager.cpp"
#include "modelsystem/sgp_ModelResource.cpp"

#include "particlesystem/sgp_ParticleSystem.cpp"
#include "particlesystem/sgp_ParticleRenderer.cpp"
#include "particlesystem/sgp_ParticleManager.cpp"


#include "instance/sgp_InstanceManager.cpp"
#include "instance/sgp_MeshComponent.cpp"
#include "instance/sgp_StaticMeshInstance.cpp"
#include "instance/sgp_SkeletonMeshInstance.cpp"

#include "effectsystem/sgp_EffectInstance.cpp"
#include "effectsystem/sgp_EffectSystemManager.cpp"

#include "renderinterface/sgp_ResourceMultiThreadLoader.cpp"

#include "font/sgp_FontManager.cpp"
}