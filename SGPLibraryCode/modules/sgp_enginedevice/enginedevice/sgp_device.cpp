
//! stub for calling createDeviceEx
SGP_API SGPDevice* SGP_CALLTYPE createDevice(SGP_DRIVER_TYPE driverType,
		uint32 windowWidth,
		uint32 windowHeight,
		uint8 bits, 
		bool fullscreen, bool stencilbuffer, bool vsync,
		bool resLoadingMT,
		Logger*	plog, ISGPEventReceiver* res)
{
	SGPCreationParameters p;
	p.DriverType = driverType;
	p.WindowWidth = windowWidth;
	p.WindowHeight = windowHeight;
	p.Bits = bits;
	p.Fullscreen = fullscreen;
	p.Stencilbuffer = stencilbuffer;
	p.Vsync = vsync;
	p.MultiThreadResLoading = resLoadingMT;
	p.EventReceiver = res;
	p.plog = plog;

	return createDeviceEx(p);
}

extern "C" SGP_API SGPDevice* SGP_CALLTYPE createDeviceEx(const SGPCreationParameters& params)
{
	SGPDevice* dev = 0;

#if SGP_WINDOWS
	dev = new (std::nothrow) SGPDeviceWin32(params);

#elif SGP_LINUX
	dev = new (std::nothrow) SGPDeviceLinux(params);

#elif SGP_MAC
	dev = new (std::nothrow) SGPDeviceMacOSX(params);

#elif SGP_IOS
#elif SGP_ANDROID

#endif

	if( dev && !dev->getRenderDevice() )
	{
		dev->closeDevice(); // destroy window
		dev->run(); // consume quit message
		delete dev;
		dev = 0;
	}

	return dev;
}
