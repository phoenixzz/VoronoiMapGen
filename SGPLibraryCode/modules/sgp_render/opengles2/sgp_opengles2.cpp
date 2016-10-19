
namespace sgp
{
	sgp_ImplementSingleton_SingleThreaded( COpenGLES2Config );

	#include "PVRTTexture/PVRTDecompress.cpp"
	#include "PVRTTexture/PVRTError.cpp"
	#include "PVRTTexture/PVRTgles2Ext.cpp"
	#include "PVRTTexture/PVRTTexture.cpp"
	#include "PVRTTexture/PVRTTextureAPI.cpp"


	#include "sgp_ImagePVRTC.cpp"
	#include "sgp_ImageLoaderPVRTC.cpp"

	#include "sgp_OpenGLES2ExtensionHandler.cpp"
	#include "sgp_OpenGLES2Camera.cpp"

	#include "sgp_OpenGLES2MaterialProperties.cpp"
	#include "sgp_OpenGLES2Material.cpp"
	#include "sgp_OpenGLES2RenderBatch.cpp"
	#include "sgp_OpenGLES2MaterialRender.cpp"
	#include "sgp_OpenGLES2FrameBufferObject.cpp"
	#include "sgp_OpenGLES2VertexBufferObject.cpp"
	#include "sgp_OpenGLES2CacheBuffer.cpp"
	#include "sgp_OpenGLES2VertexCacheManager.cpp"

	#include "sgp_OpenGLES2RenderDevice.cpp"
	#include "sgp_OpenGLES2Texture.cpp"
	#include "sgp_OpenGLSLES2Shader.cpp"
	#include "sgp_OpenGLES2ShaderManager.cpp"

	#include "sgp_OpenGLES2ParticleDynamicBuffer.cpp"
	#include "sgp_OpenGLES2ParticlePointRenderer.cpp"
	#include "sgp_OpenGLES2ParticleLineRenderer.cpp"
	#include "sgp_OpenGLES2ParticleQuadRenderer.cpp"

	#include "sgp_OpenGLES2TerrainRender.cpp"
	#include "sgp_OpenGLES2SkydomeRenderer.cpp"
	#include "sgp_OpenGLES2GrassRenderer.cpp"
	#include "sgp_OpenGLES2WaterRenderer.cpp"

	#include "sgp_OpenGLES2WorldSystemManager.cpp"

	#include "sgp_OpenGLES2FontBuffer.cpp"
	#include "sgp_OpenGLES2TTFFont.cpp"
}