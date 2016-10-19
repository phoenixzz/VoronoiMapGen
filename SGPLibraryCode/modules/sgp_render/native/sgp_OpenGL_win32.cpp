

//==============================================================================
bool OpenGLHelpers::isContextActive()
{
    return wglGetCurrentContext() != 0;
}
//==============================================================================

COpenGLRenderDevice* createOpenGLDriver(const SGPCreationParameters& params, SGPDeviceWin32* device)
{
	COpenGLRenderDevice* ogl =  new COpenGLRenderDevice(params, device);
	if (!ogl->initDriver(device))
	{
		delete ogl;
		ogl = 0;
	}
	return ogl;
}
