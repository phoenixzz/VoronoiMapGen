

#ifndef __SGP_OPENGLHELPERS_HEADER__
#define __SGP_OPENGLHELPERS_HEADER__



//==============================================================================
/**
    A set of miscellaneous openGL helper functions.
*/
class SGP_API OpenGLHelpers
{
public:
    /** Clears the GL error state. */
    static void resetErrorState();

    /** Returns true if the current thread has an active OpenGL context. */
    static bool isContextActive();

    /** Clears the current context using the given colour. */
    static void clear (const Colour& colour);


    /** Checks whether the current context supports the specified extension. */
    static bool isExtensionSupported (const char* extensionName);

    /** Returns the address of a named GL extension function */
    static void* getExtensionFunction (const char* functionName);

};


#endif   // __SGP_OPENGLHELPERS_HEADER__
