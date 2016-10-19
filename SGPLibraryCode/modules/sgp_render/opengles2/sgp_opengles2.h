#ifndef __SGP_OPENGLES2_HEADER__
#define __SGP_OPENGLES2_HEADER__

#if SGP_IOS || SGP_ANDROID || defined(BUILD_OGLES2)
 #define SGP_OPENGL_ES 1
#endif


#if defined(__APPLE__) && defined (TARGET_OS_IPHONE)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

namespace sgp
{
#include "PVRTTexture/PVRTGlobal.h"
#include "PVRTTexture/PVRTError.h"
#include "PVRTTexture/PVRTContext.h"
#include "PVRTTexture/PVRTgles2Ext.h"
#include "PVRTTexture/PVRTTexture.h"
#include "PVRTTexture/PVRTTextureAPI.h"
#include "PVRTTexture/PVRTDecompress.h"
#include "PVRTTexture/PVRTMap.h"
#include "PVRTTexture/PVRTTextureAPI.h"

#ifndef	__SGP_IMAGEPVRTC_HEADER__
	#include "sgp_ImagePVRTC.h"
#endif
#ifndef __SGP_IMAGELOADERPVRTC_HEADER__
	#include "sgp_ImageLoaderPVRTC.h"
#endif

#ifndef __SGP_OPENGLES2EXTENSION_HEADER__
	#include "sgp_OpenGLES2ExtensionHandler.h"
#endif
#ifndef __SGP_OPENGLES2CREATIONPARAMETERS_HEADER__
	#include "sgp_OpenGLES2CreationParameter.h"
#endif
#ifndef __SGP_OPENGLES2CONFIG_HEADER__
	#include "sgp_OpenGLES2Config.h"
#endif
#ifndef __SGP_OPENGLES2CAMERA_HEADER__
	#include "sgp_OpenGLES2Camera.h"
#endif
#ifndef __SGP_OPENGLES2MATERIALPROPERTIES_HEADER__
	#include "sgp_OpenGLES2MaterialProperties.h"
#endif
#ifndef __SGP_OPENGLES2RENDERBATCH_HEADER__
	#include "sgp_OpenGLES2RenderBatch.h"
#endif
#ifndef __SGP_OPENGLES2MATERIALRENDER_HEADER__
	#include "sgp_OpenGLES2MaterialRender.h"
#endif
#ifndef __SGP_OPENGLES2MATERIAL_HEADER__
	#include "sgp_OpenGLES2Material.h"
#endif
#ifndef __SGP_OPENGLES2FRAMEBUFFEROBJECT_HEADER__
	#include "sgp_OpenGLES2FrameBufferObject.h"
#endif
#ifndef __SGP_OPENGLES2VERTEXBUFFEROBJECT_HEADER__
	#include "sgp_OpenGLES2VertexBufferObject.h"
#endif
#ifndef __SGP_OPENGLES2CACHEBUFFER_HEADER__
	#include "sgp_OpenGLES2CacheBuffer.h"
#endif
#ifndef __SGP_OPENGLES2VERTEXCACHEMANAGER_HEADER__
	#include "sgp_OpenGLES2VertexCacheManager.h"
#endif

#ifndef __SGP_OPENGLES2TERRAINRENDER_HEADER__
	#include "sgp_OpenGLES2TerrainRender.h"
#endif
#ifndef __SGP_OPENGLES2SKYDOMERENDER_HEADER__
	#include "sgp_OpenGLES2SkydomeRenderer.h"
#endif
#ifndef __SGP_OPENGLES2WATERRENDER_HEADER__
	#include "sgp_OpenGLES2WaterRenderer.h"
#endif
#ifndef __SGP_OPENGLES2GRASSRENDER_HEADER__
	#include "sgp_OpenGLES2GrassRenderer.h"
#endif

#ifndef __SGP_OPENGLES2RENDERER_HEADER__
	#include "sgp_OpenGLES2RenderDevice.h"
#endif

#ifndef __SGP_OPENGLES2TEXTURE_HEADER__
	#include "sgp_OpenGLES2Texture.h"
#endif
#ifndef __SGP_OPENGLSLES2SHADER_HEADER__
	#include "sgp_OpenGLSLES2Shader.h"
#endif
#ifndef __SGP_OPENGLES2SHADERMANAGER_HEADER__
	#include "sgp_OpenGLES2ShaderManager.h"
#endif

#ifndef __SGP_OPENGLES2PARTICLEDYNAMICBUFFER_HEADER__
	#include "sgp_OpenGLES2ParticleDynamicBuffer.h"
#endif
#ifndef __SGP_OPENGLES2PARTICLEPOINTRENDERER_HEADER__
	#include "sgp_OpenGLES2ParticlePointRenderer.h"
#endif
#ifndef __SGP_OPENGLES2PARTICLELINERENDERER_HEADER__
	#include "sgp_OpenGLES2ParticleLineRenderer.h"
#endif
#ifndef __SGP_OPENGLES2PARTICLEQUADRENDERER_HEADER__
	#include "sgp_OpenGLES2ParticleQuadRenderer.h"
#endif

#ifndef __SGP_OPENGLES2WORLDSYSTEMMANAGER_HEADER__
	#include "sgp_OpenGLES2WorldSystemManager.h"
#endif

#ifndef __SGP_OPENGLES2FONTBUFFER_HEADER__
	#include "sgp_OpenGLES2FontBuffer.h"
#endif
#ifndef __SGP_OPENGES2LTTFFONT_HEADER__
	#include "sgp_OpenGLES2TTFFont.h"
#endif
}

#endif		//	__SGP_OPENGLES2_HEADER__