

//==============================================================================
#if SGP_IOS
 #import <QuartzCore/QuartzCore.h>

//==============================================================================
#elif SGP_WINDOWS
 #include <windowsx.h>

 #if SGP_MSVC
  #pragma comment(lib, "OpenGL32.Lib")
 #endif

//==============================================================================
#elif SGP_LINUX
 /* Got an include error here?

    If you want to install OpenGL support, the packages to get are "mesa-common-dev"
    and "freeglut3-dev".
 */
 #include <GL/glx.h>

//==============================================================================
#elif SGP_MAC
 #include <OpenGL/CGLCurrent.h> // These are both just needed with the 10.5 SDK
 #include <OpenGL/OpenGL.h>

//==============================================================================
#elif SGP_ANDROID
 #ifndef GL_GLEXT_PROTOTYPES
  #define GL_GLEXT_PROTOTYPES 1
 #endif
 #include <GLES2/gl2.h>
#endif

#include "GLee.c"

namespace sgp
{

////==============================================================================
//#include "../native/sgp_MissingGLDefinitions.h"
//#include "../native/sgp_OpenGLExtensions.h"
//
//void OpenGLExtensionFunctions::initialise()
//{
//   #if SGP_WINDOWS || SGP_LINUX
//    #define SGP_INIT_GL_FUNCTION(name, returnType, params, callparams) \
//        name = (type_ ## name) OpenGLHelpers::getExtensionFunction (#name);
//    #define SGP_INIT_GL_FUNCTION_EXT(name, returnType, params, callparams) \
//        name = (type_ ## name) OpenGLHelpers::getExtensionFunction (#name); \
//        if (name == nullptr) \
//            name = (type_ ## name) OpenGLHelpers::getExtensionFunction (SGP_STRINGIFY (name ## EXT));
//
//    SGP_GL_EXTENSION_FUNCTIONS (SGP_INIT_GL_FUNCTION, SGP_INIT_GL_FUNCTION_EXT)
//    #undef SGP_INIT_GL_FUNCTION
//    #undef SGP_INIT_GL_FUNCTION_EXT
//   #endif
//}
//
//#if SGP_OPENGL_ES
// #define SGP_DECLARE_GL_FUNCTION(name, returnType, params, callparams) \
//    inline returnType OpenGLExtensionFunctions::name params { return ::name callparams; }
//
// SGP_GL_EXTENSION_FUNCTIONS (SGP_DECLARE_GL_FUNCTION, SGP_DECLARE_GL_FUNCTION)
// #undef SGP_DECLARE_GL_FUNCTION
//#endif
//
//#undef SGP_GL_EXTENSION_FUNCTIONS
//
//#if SGP_OPENGL_ES
// #define SGP_MEDIUMP "mediump"
// #define SGP_HIGHP   "highp"
//#else
// #define SGP_MEDIUMP
// #define SGP_HIGHP
//#endif

//#if JUCE_DEBUG && ! defined (JUCE_CHECK_OPENGL_ERROR)
//static const char* getGLErrorMessage (const GLenum e)
//{
//    switch (e)
//    {
//        case GL_INVALID_ENUM:       return "GL_INVALID_ENUM";
//        case GL_INVALID_VALUE:      return "GL_INVALID_VALUE";
//        case GL_INVALID_OPERATION:  return "GL_INVALID_OPERATION";
//       #ifdef GL_STACK_OVERFLOW
//        case GL_STACK_OVERFLOW:     return "GL_STACK_OVERFLOW";
//       #endif
//       #ifdef GL_STACK_UNDERFLOW
//        case GL_STACK_UNDERFLOW:    return "GL_STACK_UNDERFLOW";
//       #endif
//        case GL_OUT_OF_MEMORY:      return "GL_OUT_OF_MEMORY";
//        default:                    break;
//    }
//
//    return "Unknown error";
//}
//
//static void checkGLError (const char* file, const int line)
//{
//    for (;;)
//    {
//        const GLenum e = glGetError();
//
//        if (e == GL_NO_ERROR)
//            break;
//
//        DBG ("***** " << getGLErrorMessage (e) << "  at " << file << " : " << line);
//        jassertfalse;
//    }
//}
//
// #define JUCE_CHECK_OPENGL_ERROR checkGLError (__FILE__, __LINE__);
//#else
// #define JUCE_CHECK_OPENGL_ERROR ;
//#endif

//static void clearGLError()
//{
//    while (glGetError() != GL_NO_ERROR) {}
//}

sgp_ImplementSingleton_SingleThreaded( COpenGLConfig );

//==============================================================================
#include "sgp_OpenGLExtensionHandler.cpp"
#include "sgp_OpenGLCamera.cpp"
#include "sgp_OpenGLHelpers.cpp"
#include "sgp_OpenGLRenderDevice.cpp"
#include "sgp_OpenGLTexture.cpp"
#include "sgp_OpenGLSLShader.cpp"
#include "sgp_OpenGLShaderManager.cpp"
#include "sgp_OpenGLMaterialProperties.cpp"
#include "sgp_OpenGLRenderBatch.cpp"
#include "sgp_OpenGLMaterialRender.cpp"
#include "sgp_OpenGLMaterial.cpp"
#include "sgp_OpenGLVertexBufferObject.cpp"
#include "sgp_OpenGLTextureBufferObject.cpp"
#include "sgp_OpenGLPixelBufferObject.cpp"
#include "sgp_OpenGLFrameBufferObject.cpp"
#include "sgp_OpenGLCacheBuffer.cpp"
#include "sgp_OpenGLVertexCacheManager.cpp"
#include "sgp_OpenGLParticleDynamicBuffer.cpp"
#include "sgp_OpenGLParticlePointRenderer.cpp"
#include "sgp_OpenGLParticleLineRenderer.cpp"
#include "sgp_OpenGLParticleQuadRenderer.cpp"
#include "sgp_OpenGLTerrainRender.cpp"
#include "sgp_OpenGLSkydomeRenderer.cpp"
#include "sgp_OpenGLWaterRenderer.cpp"
#include "sgp_OpenGLGrassRenderer.cpp"
#include "sgp_OpenGLWorldSystemManager.cpp"
#include "sgp_OpenGLFontBuffer.cpp"
#include "sgp_OpenGLTTFFont.cpp"

//==============================================================================
#if SGP_MAC || SGP_IOS
 #include "../../sgp_core/native/sgp_osx_ObjCHelpers.h"


 #if SGP_MAC
  #include "../native/sgp_OpenGL_osx.h"
 #else
  #include "../native/sgp_OpenGL_ios.h"
 #endif

#elif SGP_WINDOWS
 #include "../native/sgp_OpenGL_win32.cpp"

#elif SGP_LINUX
 #include "../native/sgp_OpenGL_linux.h"

#elif SGP_ANDROID
 #include "../../sgp_core/native/sgp_android_JNIHelpers.h"
 #include "../native/sgp_OpenGL_android.h"

#endif



}
