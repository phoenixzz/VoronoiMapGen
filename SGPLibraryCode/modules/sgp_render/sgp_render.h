#ifndef __SGP_RENDER_HEADER__
#define __SGP_RENDER_HEADER__

#include "../sgp_core/sgp_core.h"
#include "../sgp_math/sgp_math.h"
#include "../sgp_model/sgp_model.h"
#include "../sgp_particle/sgp_particle.h"
#include "../sgp_world/sgp_world.h"

namespace sgp
{
#ifndef __SPG_MOVEMENTCONTROLLER_HEADER__
 #include "camera/sgp_MovementController.h"
#endif
#ifndef	__SGP_DRIVERTYPE_HEADER__
 #include "renderinterface/sgp_DriverTypes.h"
#endif
#ifndef __SGP_DRIVERFEATURES_HEADER__
 #include "renderinterface/sgp_DriverFeatures.h"
#endif
#ifndef __SGP_PIXELFORMAT_HEADER__
 #include "renderinterface/sgp_PixelFormats.h"
#endif
#ifndef __SGP_VIEWPORT_HEADER__
 #include "renderinterface/sgp_Viewport.h"
#endif
#ifndef __SGP_RESOURCEMTLOADER_HEADER__
 #include "renderinterface/sgp_ResourceMultiThreadLoader.h"
#endif
#ifndef __SGP_RENDERSTAGES_HEADER__
 #include "renderinterface/sgp_RenderStages.h"
#endif

#ifndef	__SGP_IMAGE_HEADER__
 #include "texturesystem/sgp_Image.h"
#endif
#ifndef	__SGP_BLIT_HEADER__
 #include "texturesystem/sgp_Blit.h"
#endif
#ifndef	__SGP_IMAGEUNCOMPRESSED_HEADER__
 #include "texturesystem/sgp_ImageUncompressed.h"
#endif
#ifndef	__SGP_IMAGEDDS_HEADER__
 #include "texturesystem/sgp_ImageDDS.h"
#endif
#ifndef	__SGP_COLORCONVERTER_HEADER__
 #include "texturesystem/sgp_ColorConverter.h"
#endif
#ifndef __SGP_TEXTURE_HEADER__
 #include "texturesystem/sgp_Texture.h"
#endif
#ifndef __SGP_IMAGELOADER_HEADER__
 #include "texturesystem/sgp_ImageLoader.h"
#endif

#if defined (BUILD_OGLES2)

#else
	#ifndef __SGP_IMAGELOADERTGA_HEADER__
	 #include "texturesystem/sgp_ImageLoaderTGA.h"
	#endif
	#ifndef __SGP_IMAGELOADERDDS_HEADER__
	 #include "texturesystem/sgp_ImageLoaderDDS.h"
	#endif
#endif

#ifndef __SGP_TEXTURERESOURCE_HEADER__
 #include "texturesystem/sgp_TextureResource.h"
#endif
#ifndef __SGP_TEXTUREMANAGER_HEADER__
 #include "texturesystem/sgp_TextureManager.h"
#endif

#ifndef __SGP_SHADERTYPES_HEADER__
 #include "shadersystem/sgp_ShaderTypes.h"
#endif
#ifndef __SGP_SHADERMANAGER_HEADER__
 #include "shadersystem/sgp_ShaderManager.h"
#endif

#ifndef __SGP_MATERIALFLAGS_HEADER__
 #include "renderinterface/sgp_MaterialFlags.h"
#endif
#ifndef __SGP_MATERIALPROPERTIES_HEADER__
 #include "materialsystem/sgp_MaterialProperties.h"
#endif
#ifndef __SGP_ABSTRACTMATERIAL_HEADER__
 #include "materialsystem/sgp_AbstractMaterial.h"
#endif
#ifndef __SGP_MATERIALSYSTEM_HEADER__
 #include "materialsystem/sgp_MaterialSystem.h"
#endif

#ifndef __SGP_PRIMITIVETYPE_HEADER__
 #include "vertexsystem/sgp_PrimitiveTypes.h"
#endif
#ifndef __SGP_VERTEXTYPE_HEADER__
 #include "vertexsystem/sgp_VertexTypes.h"
#endif
#ifndef __SGP_HWBUFFERTYPE_HEADER__
 #include "vertexsystem/sgp_HardwareBufferType.h"
#endif
#ifndef __SGP_VERTEXBUFFERSKIN_HEADER__
 #include "vertexsystem/sgp_VertexBufferSkin.h"
#endif
#ifndef __SGP_VERTEXCACHEMANAGER_HEADER__
 #include "vertexsystem/sgp_VertexCacheManager.h"
#endif


#ifndef __SGP_PARTICLESYSTEM_HEADER__
 #include "particlesystem/sgp_ParticleSystem.h"
#endif
#ifndef __SGP_PARTICLERENDERER_HEADER__
 #include "particlesystem/sgp_ParticleRenderer.h"
#endif
#ifndef __SGP_PARTICLEMANAGER_HEADER__
 #include "particlesystem/sgp_ParticleManager.h"
#endif

#ifndef __SGP_MODELRESOURCE_HEADER__
 #include "modelsystem/sgp_ModelResource.h"
#endif
#ifndef __SGP_MODELMANAGER_HEADER__
 #include "modelsystem/sgp_ModelManager.h"
#endif

#ifndef	__SGP_INSTANCEMANAGER_HEADER__
 #include "instance/sgp_InstanceManager.h"
#endif
#ifndef	__SGP_MESHCOMPONENT_HEADER__
 #include "instance/sgp_MeshComponent.h"
#endif

#ifndef	__SGP_STATICMESHINSTANCE_HEADER__
 #include "instance/sgp_StaticMeshInstance.h"
#endif
#ifndef	__SGP_SKELETONMESHINSTANCE_HEADER__
 #include "instance/sgp_SkeletonMeshInstance.h"
#endif

#ifndef __SGP_EFFECTINSTANCE_HEADER__
 #include "effectsystem/sgp_EffectInstance.h"
#endif
#ifndef __SGP_EFFECTSYSTEMMANAGER_HEADER__
 #include "effectsystem/sgp_EffectSystemManager.h"
#endif

#ifndef	__SGP_WORLDSYSTEMMANAGER_HEADER__
 #include "worldsystem/sgp_WorldSystemManager.h"
#endif


#ifndef __SGP_FONTDRAWFLAGS_HEADER__
 #include "font/sgp_FontDrawFlags.h"
#endif
#ifndef __SGP_FONT_HEADER__
 #include "font/sgp_Font.h"
#endif
#ifndef __SGP_FONTMANAGER_HEADER__
 #include "font/sgp_FontManager.h"
#endif

#ifndef __SGP_RENDERDEVICE_HEADER__
 #include "renderinterface/sgp_RenderDevice.h"
#endif



}

#if defined (BUILD_OGLES2)
	#ifndef __SGP_OPENGLES2_HEADER__
	 #include "opengles2/sgp_opengles2.h"
	#endif
#else
	#ifndef __SGP_OPENGL_HEADER__
	 #include "opengl/sgp_opengl.h"
	#endif
#endif



#endif	// __SGP_RENDER_HEADER__