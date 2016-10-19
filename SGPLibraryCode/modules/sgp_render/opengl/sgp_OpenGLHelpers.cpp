
void OpenGLHelpers::resetErrorState()
{
    while (glGetError() != GL_NO_ERROR) {}
}

void* OpenGLHelpers::getExtensionFunction (const char* functionName)
{
   #if SGP_WINDOWS
    return (void*) wglGetProcAddress (functionName);
   #elif SGP_LINUX
    return (void*) glXGetProcAddress ((const GLubyte*) functionName);
   #else
    static void* handle = dlopen (nullptr, RTLD_LAZY);
    return dlsym (handle, functionName);
   #endif
}

bool OpenGLHelpers::isExtensionSupported (const char* const extensionName)
{
    jassert (extensionName != nullptr); // you must supply a genuine string for this.
    jassert (isContextActive()); // An OpenGL context will need to be active before calling this.

	GLint n = 0; 
	glGetIntegerv(GL_NUM_EXTENSIONS, &n); 
	for (GLint i=0; i<n; i++) 
	{ 
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		const char* found = strstr (extension, extensionName);

		if (found == nullptr)
			continue;

		return true;
	}
	return false;
}

void OpenGLHelpers::clear (const Colour& colour)
{
    glClearColor (colour.getFloatRed(), colour.getFloatGreen(),
                  colour.getFloatBlue(), colour.getFloatAlpha());

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}




