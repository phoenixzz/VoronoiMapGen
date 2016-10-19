#ifndef __SGP_OPENGL_HEADER__
#define __SGP_OPENGL_HEADER__



#undef SGP_OPENGL
#define SGP_OPENGL 1

#if SGP_IOS || SGP_ANDROID
 #define SGP_OPENGL_ES 1
#endif

#if SGP_WINDOWS
 #include "GLee.h"
#elif SGP_LINUX
 #include "GLee.h"
#elif SGP_IOS
 #include <OpenGLES/ES2/gl.h>
#elif SGP_MAC
 #include "GLee.h"
#elif SGP_ANDROID
 #include <GLES2/gl2.h>
#endif

//=============================================================================
namespace sgp
{
#ifndef __SGP_OPENGLEXTENSION_HEADER__
 #include "sgp_OpenGLExtensionHandler.h"
#endif
#ifndef __SGP_OPENGLCONFIG_HEADER__
 #include "sgp_OpenGLConfig.h"
#endif
#ifndef __SGP_OPENGLHELPERS_HEADER__
 #include "sgp_OpenGLHelpers.h"
#endif
#ifndef __SGP_OPENGLCAMERA_HEADER__
 #include "sgp_OpenGLCamera.h"
#endif
#ifndef __SGP_OPENGLMATERIALPROPERTIES_HEADER__
 #include "sgp_OpenGLMaterialProperties.h"
#endif

#ifndef __SGP_OPENGLRENDERBATCH_HEADER__
 #include "sgp_OpenGLRenderBatch.h"
#endif
#ifndef __SGP_OPENGLMATERIALRENDER_HEADER__
 #include "sgp_OpenGLMaterialRender.h"
#endif
#ifndef __SGP_OPENGLTERRAINRENDER_HEADER__
 #include "sgp_OpenGLTerrainRender.h"
#endif
#ifndef __SGP_OPENGLSKYDOMERENDER_HEADER__
 #include "sgp_OpenGLSkydomeRenderer.h"
#endif
#ifndef __SGP_OPENGLWATERRENDER_HEADER__
 #include "sgp_OpenGLWaterRenderer.h"
#endif
#ifndef __SGP_OPENGLGRASSRENDER_HEADER__
 #include "sgp_OpenGLGrassRenderer.h"
#endif

#ifndef __SGP_OPENGLFRAMEBUFFEROBJECT_HEADER__
 #include "sgp_OpenGLFrameBufferObject.h"
#endif
#ifndef __SGP_OPENGLTEXTUREBUFFEROBJECT_HEADER__
 #include "sgp_OpenGLTextureBufferObject.h"
#endif
#ifndef __SGP_OPENGLPIXELBUFFEROBJECT_HEADER__
 #include "sgp_OpenGLPixelBufferObject.h"
#endif
#ifndef __SGP_OPENGLVERTEXBUFFEROBJECT_HEADER__
 #include "sgp_OpenGLVertexBufferObject.h"
#endif

#ifndef __SGP_OPENGLCACHEBUFFER_HEADER__
 #include "sgp_OpenGLCacheBuffer.h"
#endif

#ifndef __SGP_OPENGLVERTEXCACHEMANAGER_HEADER__
 #include "sgp_OpenGLVertexCacheManager.h"
#endif

#ifndef __SGP_OPENGLRENDERER_HEADER__
 #include "sgp_OpenGLRenderDevice.h"
#endif

#ifndef __SGP_OPENGLTEXTURE_HEADER__
 #include "sgp_OpenGLTexture.h"
#endif
#ifndef __SGP_OPENGLSLSHADER_HEADER__
 #include "sgp_OpenGLSLShader.h"
#endif
#ifndef __SGP_OPENGLSHADERMANAGER_HEADER__
 #include "sgp_OpenGLShaderManager.h"
#endif
#ifndef __SGP_OPENGLMATERIAL_HEADER__
 #include "sgp_OpenGLMaterial.h"
#endif



#ifndef __SGP_OPENGLPARTICLEDYNAMICBUFFER_HEADER__
 #include "sgp_OpenGLParticleDynamicBuffer.h"
#endif
#ifndef __SGP_OPENGLPARTICLEPOINTRENDERER_HEADER__
 #include "sgp_OpenGLParticlePointRenderer.h"
#endif
#ifndef __SGP_OPENGLPARTICLELINERENDERER_HEADER__
 #include "sgp_OpenGLParticleLineRenderer.h"
#endif
#ifndef __SGP_OPENGLPARTICLEQUADRENDERER_HEADER__
 #include "sgp_OpenGLParticleQuadRenderer.h"
#endif

#ifndef __SGP_OPENGLWORLDSYSTEMMANAGER_HEADER__
 #include "sgp_OpenGLWorldSystemManager.h"
#endif


#ifndef __SGP_OPENGLFONTBUFFER_HEADER__
 #include "sgp_OpenGLFontBuffer.h"
#endif
#ifndef __SGP_OPENGLTTFFONT_HEADER__
 #include "sgp_OpenGLTTFFont.h"
#endif
}


#endif   // __SGP_OPENGL_HEADER__
