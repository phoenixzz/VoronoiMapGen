

#if SGP_WINDOWS
//! Windows constructor and init code
COpenGLRenderDevice::COpenGLRenderDevice(const SGPCreationParameters& params, SGPDeviceWin32* device)
: COpenGLExtensionHandler(),
	m_ClearRedColor(0.0f), m_ClearGreenColor(0.0f), m_ClearBlueColor(0.0f), m_ClearAlphaColor(1.0f),
	m_AntiAlias(params.AntiAlias), m_PixelFormat(SGPPF_R8G8B8), m_RenderStage(SGPRS_NORMAL),
	m_Params(params),
	HDc(0), BackUpHDc(0),
	HRc(0), BackUpHRc(0),
	Window(static_cast<HWND>(params.WindowId)),
	BackUpWindow(static_cast<HWND>(params.WindowId)),
	Win32Device(device),
	m_pLogger(params.plog),
	m_pCurrentCamera(NULL),
	m_pMTResourceLoader(NULL),
	m_pInstanceManager(NULL), m_pShaderManager(NULL), m_pMaterialSystem(NULL),
	m_pVertexCacheManager(NULL), m_pModelManager(NULL),	m_pParticleManager(NULL),
	m_pTextureManager(NULL),
	m_pEffectSystemManager(NULL), m_pWorldSystemManager(NULL),
	m_pFontManager(NULL),
	m_pMaterialRenderer(NULL), m_pTerrainRenderer(NULL), m_pSkydomeRenderer(NULL),
	m_pWaterRenderer(NULL), m_pGrassRenderer(NULL),
	m_pSceneFBO(NULL)
{
	m_pOpenGLConfig = COpenGLConfig::getInstance();
	jassert(m_pLogger);
}

bool COpenGLRenderDevice::changeRenderContext(const HDC _HDc, const HGLRC _HRc, const HWND _Window, SGPDeviceWin32* )
{
	if (_Window && _HDc && _HRc)
	{
		if (!wglMakeCurrent(_HDc, _HRc))
		{
			m_pLogger->writeToLog(String("Render Context switch failed."), ELL_INFORMATION);
			return false;
		}
		else
		{
			HDc = _HDc;
		}
	}
	// set back to main context
	else if (HDc != BackUpHDc)
	{
		if (!wglMakeCurrent(BackUpHDc, BackUpHRc))
		{
			m_pLogger->writeToLog(String("Render Context switch failed."), ELL_INFORMATION);
			return false;
		}
		else
		{
			HDc = BackUpHDc;
		}
	}
	return true;
}

//! inits the open gl driver
bool COpenGLRenderDevice::initDriver(SGPDeviceWin32* device)
{
	// Create a window to test antialiasing support
	const wchar_t* ClassName = TEXT("GLSGPDeviceWin32");
	HINSTANCE lhInstance = GetModuleHandle(0);

	// Register Class
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)DefWindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = lhInstance;
	wcex.hIcon         = NULL;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = 0;
	wcex.lpszClassName = ClassName;
	wcex.hIconSm       = 0;
	wcex.hIcon         = 0;
	RegisterClassEx(&wcex);

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = m_Params.WindowWidth;
	clientSize.bottom = m_Params.WindowHeight;

	DWORD style = WS_POPUP;
	if (!m_Params.Fullscreen)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	AdjustWindowRect(&clientSize, style, FALSE);

	const uint32 realWidth = clientSize.right - clientSize.left;
	const uint32 realHeight = clientSize.bottom - clientSize.top;

	const uint32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const uint32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	HWND temporary_wnd = CreateWindow(ClassName, TEXT(""), style, windowLeft,
			windowTop, realWidth, realHeight, NULL, NULL, lhInstance, NULL);

	if (!temporary_wnd)
	{
		m_pLogger->writeToLog(String("Cannot create a temporary window."), ELL_ERROR);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}

	HDc = GetDC(temporary_wnd);

	// Set up pixel format descriptor with desired parameters
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),             // Size Of This Pixel Format Descriptor
		1,                                         // Version Number
		PFD_DRAW_TO_WINDOW |                       // Format Must Support Window
		PFD_SUPPORT_OPENGL |                       // Format Must Support OpenGL
		(m_Params.Doublebuffer?PFD_DOUBLEBUFFER:0) | // Must Support Double Buffering
		0,										   // NOT Support Stereo Buffer
		PFD_TYPE_RGBA,                             // Request An RGBA Format
		m_Params.Bits,                             // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                          // Color Bits Ignored
		0,                                         // No Alpha Buffer
		0,                                         // Shift Bit Ignored
		0,                                         // No Accumulation Buffer
		0, 0, 0, 0,	                               // Accumulation Bits Ignored
		m_Params.ZBufferBits,                      // Z-Buffer (Depth Buffer)
		BYTE(m_Params.Stencilbuffer ? 1 : 0),      // Stencil Buffer Depth
		0,                                         // No Auxiliary Buffer
		PFD_MAIN_PLANE,                            // Main Drawing Layer
		0,                                         // Reserved
		0, 0, 0                                    // Layer Masks Ignored
	};

	GLuint PixelFormat = 0;

	for (uint32 i=0; i<6; ++i)
	{
		if (i == 1)
		{
			if (m_Params.Stencilbuffer)
			{
				m_pLogger->writeToLog(String("Cannot create a GL device with stencil buffer."), ELL_WARNING);
				m_Params.Stencilbuffer = false;
				pfd.cStencilBits = 0;
			}
			else
				continue;
		}
		else if (i == 2)
		{
			pfd.cDepthBits = 24;
		}
		else if (i == 3)
		{
			if (m_Params.Bits != 16)
				pfd.cDepthBits = 16;
			else
				continue;
		}
		else if (i == 4)
		{
			// try single buffer
			if (m_Params.Doublebuffer)
				pfd.dwFlags &= ~PFD_DOUBLEBUFFER;
			else
				continue;
		}
		else if (i == 5)
		{
			m_pLogger->writeToLog(String("Cannot create a GL device context -- No suitable format for temporary window."), ELL_ERROR);
			ReleaseDC(temporary_wnd, HDc);
			DestroyWindow(temporary_wnd);
			UnregisterClass(ClassName, lhInstance);
			return false;
		}

		// choose pixelformat
		PixelFormat = ChoosePixelFormat(HDc, &pfd);
		if (PixelFormat)
			break;
	}

	SetPixelFormat(HDc, PixelFormat, &pfd);
	HGLRC hrc = wglCreateContext(HDc);
	if (!hrc)
	{
		m_pLogger->writeToLog(String("Cannot create a temporary GL rendering context."), ELL_ERROR);
		ReleaseDC(temporary_wnd, HDc);
		DestroyWindow(temporary_wnd);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}


	if (!changeRenderContext(HDc, hrc, temporary_wnd, device))
	{
		m_pLogger->writeToLog(String("Cannot activate a temporary GL rendering context."), ELL_ERROR);
		wglDeleteContext(hrc);
		ReleaseDC(temporary_wnd, HDc);
		DestroyWindow(temporary_wnd);
		UnregisterClass(ClassName, lhInstance);
		return false;
	}

	const bool pixel_format_supported = GLEE_WGL_ARB_pixel_format ? true : false;
	const bool multi_sample_supported = GLEE_WGL_ARB_multisample || GLEE_WGL_EXT_multisample || GLEE_WGL_3DFX_multisample;

#ifdef WGL_ARB_pixel_format
	if (pixel_format_supported)
	{
		// This value determines the number of samples used for antialiasing
		// My experience is that 8 does not show a big
		// improvement over 4, but 4 shows a big improvement
		// over 2.

		if(m_AntiAlias > 32)
			m_AntiAlias = 32;

		float fAttributes[] = {0.0, 0.0};
		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, 1,
			WGL_SUPPORT_OPENGL_ARB, 1,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, (m_Params.Bits == 32) ? 24 : 15,
			WGL_ALPHA_BITS_ARB, (m_Params.Bits == 32) ? 8 : 1,
			WGL_DEPTH_BITS_ARB, m_Params.ZBufferBits, // 10,11
			WGL_STENCIL_BITS_ARB, m_Params.Stencilbuffer ? 1 : 0,
			WGL_DOUBLE_BUFFER_ARB, m_Params.Doublebuffer ? 1 : 0,
			WGL_STEREO_ARB, 0,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
#ifdef WGL_ARB_multisample
			WGL_SAMPLES_ARB, m_AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_ARB, 1,
#elif defined(WGL_EXT_multisample)
			WGL_SAMPLES_EXT, m_AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_EXT, 1,
#elif defined(WGL_3DFX_multisample)
			WGL_SAMPLES_3DFX, m_AntiAlias, // 20,21
			WGL_SAMPLE_BUFFERS_3DFX, 1,
#endif
#ifdef WGL_ARB_framebuffer_sRGB
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, m_Params.HandleSRGB ? 1:0,
#elif defined(WGL_EXT_framebuffer_sRGB)
			WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT, m_Params.HandleSRGB ? 1:0,
#endif
//			WGL_DEPTH_FLOAT_EXT, 1,
			0,0,0,0
		};
		int iAttrSize = sizeof(iAttributes) / sizeof(int);
		const bool framebuffer_srgb_supported = GLEE_WGL_ARB_framebuffer_sRGB || GLEE_WGL_EXT_framebuffer_sRGB;
		if (!framebuffer_srgb_supported)
		{
			memmove(&iAttributes[24],&iAttributes[26],sizeof(int)*(iAttrSize-26));
			iAttrSize -= 2;
		}
		if (!multi_sample_supported)
		{
			memmove(&iAttributes[20],&iAttributes[24],sizeof(int)*(iAttrSize-24));
			iAttrSize -= 4;
		}

		int32 rv = 0;
		// Try to get an acceptable pixel format
		do
		{
			int pixelFormat = 0;
			UINT numFormats = 0;
			const BOOL valid = wglChoosePixelFormatARB(HDc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

			if (valid && numFormats)
				rv = pixelFormat;
			else
				iAttributes[21] -= 1;
		}
		while( rv==0 && iAttributes[21]>1 );
		if (rv)
		{
			PixelFormat = rv;
			m_AntiAlias = iAttributes[21];
		}
	}
	else
#endif
		m_AntiAlias = 0;

	wglMakeCurrent(HDc, NULL);
	wglDeleteContext(hrc);
	ReleaseDC(temporary_wnd, HDc);
	DestroyWindow(temporary_wnd);
	UnregisterClass(ClassName, lhInstance);

	// get hdc
	HDc = GetDC(Window);
	if (!HDc)
	{
		m_pLogger->writeToLog(String("Cannot create a GL device context."), ELL_ERROR);
		return false;
	}

	// search for pixel format the simple way
	if (PixelFormat==0 || (!SetPixelFormat(HDc, PixelFormat, &pfd)))
	{
		for (uint32 i=0; i<5; ++i)
		{
			if (i == 1)
			{
				if (m_Params.Stencilbuffer)
				{
					m_pLogger->writeToLog(String("Cannot create a GL device with stencil buffer."), ELL_WARNING);
					m_Params.Stencilbuffer = false;
					pfd.cStencilBits = 0;
				}
				else
					continue;
			}
			else if (i == 2)
			{
				pfd.cDepthBits = 24;
			}
			if (i == 3)
			{
				if (m_Params.Bits != 16)
					pfd.cDepthBits = 16;
				else
					continue;
			}
			else if (i == 4)
			{
				m_pLogger->writeToLog(String("Cannot create a GL device context -- No suitable format."), ELL_ERROR);
				return false;
			}

			// choose pixelformat
			PixelFormat = ChoosePixelFormat(HDc, &pfd);
			if (PixelFormat)
				break;
		}
	}

	// set pixel format
	if (!SetPixelFormat(HDc, PixelFormat, &pfd))
	{
		m_pLogger->writeToLog(String("Cannot set the pixel format."), ELL_ERROR);
		return false;
	}
	m_pLogger->writeToLog(String("Pixel Format : ") + String((int)PixelFormat), ELL_DEBUG);

	// create rendering context
#ifdef WGL_ARB_create_context
	if (GLEE_WGL_ARB_create_context)
	{
		int iAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		hrc = wglCreateContextAttribsARB(HDc, 0, iAttribs);
	}
	else
#endif
		hrc = wglCreateContext(HDc);

	if (!hrc)
	{
		m_pLogger->writeToLog(String("Cannot create a GL rendering context."), ELL_ERROR);
		return false;
	}

	// set exposed data
	BackUpHDc = HDc;
	BackUpHRc = hrc;
	BackUpWindow = Window;

	// activate rendering context
	if (!changeRenderContext(BackUpHDc, BackUpHRc, BackUpWindow, device))
	{
		m_pLogger->writeToLog(String("Cannot activate GL rendering context"), ELL_ERROR);
		wglDeleteContext(hrc);
		return false;
	}

	int pf = GetPixelFormat(HDc);
	DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.cAlphaBits != 0)
	{
		if (pfd.cRedBits == 8)
			m_PixelFormat = SGPPF_A8R8G8B8;
		else if (pfd.cRedBits == 4)
			m_PixelFormat = SGPPF_A4R4G4B4;
		else
			m_PixelFormat = SGPPF_A1R5G5B5;
	}
	else
	{
		if (pfd.cRedBits == 8)
			m_PixelFormat = SGPPF_R8G8B8;
		else
			m_PixelFormat = SGPPF_R5G6B5;
	}

	if( !OnDriverInit() )
	{
		return false;
	}

	extGlSwapInterval(m_Params.Vsync ? 1 : 0);
	return true;
}



COpenGLRenderDevice::~COpenGLRenderDevice()
{
	COpenGLConfig::deleteInstance();
	
	if( m_pMTResourceLoader )
	{
		delete m_pMTResourceLoader;
		m_pMTResourceLoader = NULL;
	}

	if( m_pInstanceManager )
	{
		delete m_pInstanceManager;
		m_pInstanceManager = NULL;
	}

	if( m_pShaderManager )
	{
		delete m_pShaderManager;
		m_pShaderManager = NULL;
	}
	if( m_pMaterialSystem )
	{
		delete m_pMaterialSystem;
		m_pMaterialSystem = NULL;
	}
	if( m_pVertexCacheManager )
	{
		delete m_pVertexCacheManager;
		m_pVertexCacheManager = NULL;
	}
	if( m_pModelManager )
	{
		delete m_pModelManager;
		m_pModelManager = NULL;
	}
	if( m_pParticleManager )
	{
		delete m_pParticleManager;
		m_pParticleManager = NULL;
	}
	if( m_pEffectSystemManager )
	{
		delete m_pEffectSystemManager;
		m_pEffectSystemManager = NULL;
	}
	if( m_pWorldSystemManager )
	{
		delete m_pWorldSystemManager;
		m_pWorldSystemManager = NULL;
	}

	if( m_pCurrentCamera )
	{
		delete m_pCurrentCamera;
		m_pCurrentCamera = NULL;
	}
	if( m_pMaterialRenderer )
	{
		delete m_pMaterialRenderer;
		m_pMaterialRenderer = NULL;
	}
	if( m_pTerrainRenderer)
	{
		delete m_pTerrainRenderer;
		m_pTerrainRenderer = NULL;
	}
	if( m_pSkydomeRenderer )
	{
		delete m_pSkydomeRenderer;
		m_pSkydomeRenderer = NULL;
	}
	if( m_pWaterRenderer )
	{
		delete m_pWaterRenderer;
		m_pWaterRenderer = NULL;
	}
	if( m_pGrassRenderer )
	{
		delete m_pGrassRenderer;
		m_pGrassRenderer = NULL;
	}
	if( m_pFontManager )
	{
		delete m_pFontManager;
		m_pFontManager = NULL;
	}


	// Texture manager should release last
	if( m_pTextureManager )
	{
		delete m_pTextureManager;
		m_pTextureManager = NULL;
	}

	if (BackUpHRc)
	{
		if (!wglMakeCurrent(HDc, 0))
			m_pLogger->writeToLog(String("Release of dc and rc failed."), ELL_WARNING);

		if (!wglDeleteContext(BackUpHRc))
			m_pLogger->writeToLog(String("Release of rendering context failed."), ELL_WARNING);
	}

	if (HDc)
		ReleaseDC(Window, HDc);
}
#endif



bool COpenGLRenderDevice::OnDriverInit()
{
	// load extensions
	initExtensions(m_Params.Stencilbuffer);

	char *versionStr = (char *)glGetString(GL_VERSION);
	m_VideoDriverName = String("Using renderer - OpenGL Ver : ") + String(versionStr);

	m_pLogger->writeToLog(getVideoDriverName(), ELL_INFORMATION);
	
	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		m_VendorName = String( reinterpret_cast<const char*>(vendor) );
		m_pLogger->writeToLog( String(reinterpret_cast<const char*>(renderer)) + m_VendorName, ELL_INFORMATION);
	}

	if(queryDriverFeature(SGPVDF_ARB_GLSL))
	{
		m_pLogger->writeToLog(String("GLSL version : ") + String(double(ShaderLanguageVersion)*0.01), ELL_INFORMATION);
	}
	else
		m_pLogger->writeToLog(String("GLSL not available."), ELL_INFORMATION);

	m_pOpenGLConfig->MaxTextureUnits = MaxTextureUnits;
	m_pOpenGLConfig->MaxAnisotropy = MaxAnisotropy;
	m_pOpenGLConfig->MaxUserClipPlanes = MaxUserClipPlanes;
	m_pOpenGLConfig->MaxAuxBuffers = MaxAuxBuffers;
	m_pOpenGLConfig->MaxMultipleRenderTargets = MaxMultipleRenderTargets;
	m_pOpenGLConfig->MaxColorAttachementPoints = MaxColorAttachementPoints;
	m_pOpenGLConfig->FullScreenAntiAlias = m_AntiAlias;
	m_pOpenGLConfig->MaxTextureSize = MaxTextureSize;
	m_pOpenGLConfig->MaxTextureLODBias = MaxTextureLODBias;

	m_pOpenGLConfig->OpenGLVersion = Version;
	m_pOpenGLConfig->GLSLVersion = ShaderLanguageVersion;
	m_pOpenGLConfig->HandleSRGB = m_Params.HandleSRGB;

	if( Version < 330 )
	{
		m_pLogger->writeToLog(String("Cannot create OpenGL Driver because poor Graphic Card or lower Driver."), ELL_ERROR);
		return false;
	}



	// Some OPENGL init setting
	// Reset The Current Viewport
	glViewport(0, 0, m_Params.WindowWidth, m_Params.WindowHeight);

	setAmbientLight( Colour(0,0,0,0.0f) );

	// Init Render State
	InitOpenGLRenderState();

	// Manager System Init
	// Texture System
	m_pTextureManager = new CSGPTextureManager(this,m_pLogger);
	m_pTextureManager->createDefaultTexture();
	m_pTextureManager->createWhiteTexture();
	m_pTextureManager->createBlackTexture();

	// Shader System
	m_pShaderManager = new COpenGLShaderManager(this,m_pLogger);

	// Camera System
	m_pCurrentCamera = new COpenGLCamera(this);

	// Material System
	m_pMaterialSystem = new ISGPMaterialSystem(this);
	m_pMaterialSystem->LoadGameMaterials();

	// Material Renderer
	m_pMaterialRenderer = new COpenGLMaterialRenderer(this);

	// Terrain Renderer
	m_pTerrainRenderer = new COpenGLTerrainRenderer(this);

	// Skydome Renderer
	m_pSkydomeRenderer = new COpenGLSkydomeRenderer(this);

	// Water Renderer
	m_pWaterRenderer = new COpenGLWaterRenderer(this);

	// Grass Renderer
	m_pGrassRenderer = new COpenGLGrassRenderer(this);

	// VertexCache System
	m_pVertexCacheManager = new COpenGLVertexCacheManager(this);

	// Model System
	m_pModelManager = new ISGPModelManager(this,m_pLogger);

	// Particle System
	m_pParticleManager = new ISGPParticleManager(this,m_pLogger);

	// Effect Instance System
	m_pEffectSystemManager = new ISGPEffectSystemManager(this,m_pLogger);

	// World System
	m_pWorldSystemManager = new COpenGLWorldSystemManager(this,m_pLogger);

	// Font System
	m_pFontManager = new ISGPFontManager(this);

	// Multi-Thread Resource Loader
	m_pMTResourceLoader = new CSGPResourceLoaderMuitiThread(this);

	// Instance Manager
	m_pInstanceManager = new ISGPInstanceManager(this);


	return true;
}

void COpenGLRenderDevice::InitOpenGLRenderState()
{
	// Set the depth buffer to be entirely cleared to 1.0 values.
    glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glShadeModel(GL_SMOOTH);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

    // Set the polygon winding to front facing for the right handed system.
    glFrontFace(GL_CCW);

    // Enable back face culling.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );


//#ifdef GL_EXT_separate_specular_color
//	if(GLEE_EXT_separate_specular_color)
//		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
//#endif
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	m_Params.HandleSRGB &= ((GLEE_ARB_framebuffer_sRGB || GLEE_EXT_framebuffer_sRGB) &&	GLEE_EXT_texture_sRGB);
#if defined(GL_ARB_framebuffer_sRGB)
	if (m_Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);
#elif defined(GL_EXT_framebuffer_sRGB)
	if (m_Params.HandleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
#endif

}



void COpenGLRenderDevice::setClearColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_ClearRedColor = fRed;
	m_ClearGreenColor = fGreen;
	m_ClearBlueColor = fBlue;
	m_ClearAlphaColor = fAlpha;
}

//! init call for rendering start
bool COpenGLRenderDevice::beginScene( bool bClearColorBuffer, bool bClearDepthBuffer, bool bClearStencilBuffer )
{
	GLbitfield mask = 0;
	if (bClearColorBuffer)
	{
		glClearColor(m_ClearRedColor, m_ClearGreenColor, m_ClearBlueColor, m_ClearAlphaColor);
		mask |= GL_COLOR_BUFFER_BIT;
	}
	if (bClearDepthBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (bClearStencilBuffer)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
		glClear(mask);

	getOpenGLMaterialRenderer()->BeforeDrawRenderBatch();

	return true;
}

//! presents the rendered scene on the screen, returns false if failed
bool COpenGLRenderDevice::endScene()
{
	glFlush();

#if SGP_WINDOWS
	return SwapBuffers(HDc) != FALSE;
#elif SGP_LINUX
	glXSwapBuffers(X11Display, Drawable);
	return true;
#elif SGP_MAC
	OSXDevice->flush();
	return true;


#elif SGP_IOS
	return false;
#elif SGP_ANDROID
	return false;
#endif


}

void COpenGLRenderDevice::clearZBuffer()
{
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
}


void COpenGLRenderDevice::createRenderToFrameBuffer( uint32 Width, uint32 Height, bool stencilbuffer )
{
	// First calculate how many Color Attachment must be created for FBO
	uint32 nColorAttachmentCount = 1;
	nColorAttachmentCount += (GetWorldSystemManager()->isUsingDepthTexture() ? 1 : 0) +
		(GetWorldSystemManager()->isUsingRefractionMap() ? 1 : 0);
	

	m_pSceneFBO = new COpenGLFrameBufferObject(this);
	m_pSceneFBO->createFBO(String("MainScene"), nColorAttachmentCount, Width, Height, true, stencilbuffer);
	
	if( GetWorldSystemManager()->isUsingReflectionMap() )
		m_pSceneFBO->createAuxiliaryTextureToFBO(String("QuarterReflectionMap"), Width, Height);
}

void COpenGLRenderDevice::recreateRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer)
{
	// First calculate how many Color Attachment must be created for new FBO
	uint32 nColorAttachmentCount = 1;
	nColorAttachmentCount += (GetWorldSystemManager()->isUsingDepthTexture() ? 1 : 0) +
		(GetWorldSystemManager()->isUsingRefractionMap() ? 1 : 0);

	if( m_pSceneFBO && 
		(m_pSceneFBO->getColorAttachmentNumber() == nColorAttachmentCount) &&
		(m_pSceneFBO->getWidth() == Width) &&
		(m_pSceneFBO->getHeight() == Height) )
		return;

	if( m_pSceneFBO )
		deleteRenderToFrameBuffer();

	m_pSceneFBO = new COpenGLFrameBufferObject(this);
	m_pSceneFBO->createFBO(String("MainScene"), nColorAttachmentCount, Width, Height, true, stencilbuffer);
	
	if( GetWorldSystemManager()->isUsingReflectionMap() )
		m_pSceneFBO->createAuxiliaryTextureToFBO(String("QuarterReflectionMap"), Width, Height);
}

void COpenGLRenderDevice::setRenderToFrameBuffer()
{
	if( m_pSceneFBO )
		m_pSceneFBO->bindFBO( false );
}

void COpenGLRenderDevice::renderBackToMainBuffer()
{
	if( m_pSceneFBO )
	{
		m_pSceneFBO->unbindFBO();
		m_pSceneFBO->bindFramebufferTexture(0, 0, false);

		GetVertexCacheManager()->RenderFullScreenQuad();
	}
}

void COpenGLRenderDevice::deleteRenderToFrameBuffer()
{
	if( m_pSceneFBO )
	{
		delete m_pSceneFBO;
		m_pSceneFBO = NULL;
	}
}


void COpenGLRenderDevice::onResize(const uint32 width, const uint32 height)
{
	if( m_ViewPort.Width == m_ScreenSize.Width &&
		m_ViewPort.Height == m_ScreenSize.Height )
	{
		m_ViewPort.X = m_ViewPort.Y = 0;
		m_ViewPort.Width = width;
		m_ViewPort.Height = height;
	}
	m_ScreenSize.Width = width;
	m_ScreenSize.Height = height;

	setViewPort(m_ViewPort);
}

// returns the current size of the screen or rendertarget
const SDimension2D& COpenGLRenderDevice::getCurrentRenderTargetSize() const
{
	if (m_CurrentRTSize.Width == 0)
		return m_ScreenSize;
	else
		return m_CurrentRTSize;
}


void COpenGLRenderDevice::setCameraMode(SGP_CAMERAMODE_TYPE Mode)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->setCameraMode(Mode);
}

void COpenGLRenderDevice::setFov(float fov)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->m_fFov = fov;
}

void COpenGLRenderDevice::setViewPort(const SViewPort& area)
{
	m_ViewPort.X = area.X;
	m_ViewPort.Y = area.Y;
	m_ViewPort.Width = area.Width;
	m_ViewPort.Height = area.Height;

	setProjMatrixParams((float)m_ViewPort.Width/(float)m_ViewPort.Height);
			
	glViewport(m_ViewPort.X, m_ViewPort.Y, m_ViewPort.Width, m_ViewPort.Height);
}

void COpenGLRenderDevice::setNearFarClipPlane(float fNear, float fFar)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->SetClippingPlanes( fNear, fFar );
}
/**
 * Calculate perspective and orthogonal projection matrix for current camera
 * using given values.
 * -> IN: float       - field of view (horizontal)(in degree)
 *        float		  - aspect ration (viewport height / width)
 */
void COpenGLRenderDevice::setProjMatrixParams(float fAspect)
{
	if(!m_pCurrentCamera)
		return;

	if( m_pCurrentCamera->m_Mode == SGPCT_PERSPECTIVE )
	{
		// PERSPECTIVE PROJECTION MATRIX
		m_pCurrentCamera->CalcPerspProjMatrix(fAspect);
	}
	else if( m_pCurrentCamera->m_Mode == SGPCT_ORTHOGONAL )
	{
		m_pCurrentCamera->CalcOrthoProjMatrix(
			(float)m_ViewPort.X, (float)m_ViewPort.X+m_ViewPort.Width,
			(float)m_ViewPort.Y, (float)m_ViewPort.Y+m_ViewPort.Height,
			m_pCurrentCamera->m_fNear,
			m_pCurrentCamera->m_fFar );
	}
	else if( m_pCurrentCamera->m_Mode == SGPCT_TWOD )
	{
		m_pCurrentCamera->Prepare2DMode();
	}
}

void COpenGLRenderDevice::setViewMatrix3D( 
	const Vector4D& vcRight, const Vector4D& vcUp, const Vector4D& vcDir, const Vector4D& vcEyePos )
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->SetViewMatrix3D(vcRight, vcUp, vcDir, vcEyePos);
}

/**
 * Calculates current camera View matrix for given point and calls SetViewMatrix3D
 * to active the look at matrix.
 * -> IN: Vector4D& - 4 floats building camera's position vector
 *        Vector4D& - 4 floats building lookat point vector
 *        Vector4D& - 4 floats building world up vector at cam pos
 */
void COpenGLRenderDevice::setViewMatrixLookAt( 
	const Vector4D& vcPos, const Vector4D& vcPoint, const Vector4D& vcWorldUp) 
{
	Vector4D vcDir, vcTemp, vcUp;

	vcDir = vcPoint - vcPos;
	vcDir.Normalize();

	// calculate up vector
	float fDot = vcWorldUp * vcDir;
 	vcTemp = vcDir * fDot;
	vcUp = vcWorldUp - vcTemp;
	float fL = vcUp.GetLength();

	// if length too small take normal y axis as up vector
	if(fL < 1e-6f) 
	{
		Vector4D vcY;
		vcY.Set(0.0f, 1.0f, 0.0f);

		vcTemp = vcDir * vcDir.y;
		vcUp = vcY - vcTemp;

		fL = vcUp.GetLength();

		// if still too small take z axis as up vector
		if (fL < 1e-6f) 
		{
			vcY.Set(0.0f, 0.0f, 1.0f);

			vcTemp = vcDir * vcDir.z;
			vcUp = vcY - vcTemp;

			// if still too small we are lost         
			fL = vcUp.GetLength();
			if(fL < 1e-6f) 
				return;
		}
	}

	vcUp /= fL;

	// build right vector using cross product
	Vector4D vcRight;
	vcRight.Cross(vcUp, vcDir);
	vcRight.Normalize();

	vcUp.Cross(vcDir, vcRight);

	return setViewMatrix3D(vcRight, vcUp, vcDir, vcPos);
}

void COpenGLRenderDevice::getCamreaPosition(Vector4D* pCameraPos)
{
	if( m_pCurrentCamera && pCameraPos )
	{
		pCameraPos->Set(m_pCurrentCamera->GetPos());
	}
}

void COpenGLRenderDevice::getCamreaViewDirection(Vector3D* pViewDir)
{
	if( m_pCurrentCamera && pViewDir )
	{
		pViewDir->Set(m_pCurrentCamera->m_mViewMatrix._13, m_pCurrentCamera->m_mViewMatrix._23, m_pCurrentCamera->m_mViewMatrix._33);
	}
}

void COpenGLRenderDevice::getViewFrustrum(Plane *pPlane)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetFrustrum(pPlane);
}

void COpenGLRenderDevice::getViewMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetViewMatrix(mat);
}

void COpenGLRenderDevice::getProjMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetProjMatrix(mat);
}

void COpenGLRenderDevice::getViewProjMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetViewProjMatrix(mat);
}

/**
 * Cast a world ray from a given position on screen.
 * -> IN:  POINT       - point in screen coordinates
 * -> OUT: Vector4D& - to fill with 4 floats for ray origin vector
 *         Vector4D& - to fill with 4 floats for ray direction
 */
void COpenGLRenderDevice::Transform2Dto3D(const SDimension2D &pt, Vector4D *vcOrig, Vector4D *vcDir)
{
	Matrix4x4 *pView = NULL;
	Matrix4x4 *pProj = NULL;
	Matrix4x4 mInvView;
	Vector4D vcS;
	uint32 dwWidth, dwHeight;

	if( m_pCurrentCamera )
	{
		dwWidth  = m_ViewPort.Width;
		dwHeight = m_ViewPort.Height;

		pView = &m_pCurrentCamera->m_mViewMatrix;
		pProj = &m_pCurrentCamera->m_mProjMatrix;

		// resize to viewportspace [-1,1] -> projection
		vcS.x =  ( ((pt.Width*2.0f) / dwWidth) -1.0f) / pProj->_11;
		vcS.y = -( ((pt.Height*2.0f) / dwHeight)-1.0f) / pProj->_22;
		vcS.z = 1.0f;

		// invert view matrix
		mInvView.InverseOf(*pView);

		// ray from screen to worldspace
		(*vcDir).x = (vcS.x * mInvView._11)	+ (vcS.y * mInvView._21) + (vcS.z * mInvView._31);
		(*vcDir).y = (vcS.x * mInvView._12)	+ (vcS.y * mInvView._22) + (vcS.z * mInvView._32);
		(*vcDir).z = (vcS.x * mInvView._13)	+ (vcS.y * mInvView._23) + (vcS.z * mInvView._33);
   
		// inverse translation.
		(*vcOrig).x = mInvView._41;
		(*vcOrig).y = mInvView._42;
		(*vcOrig).z = mInvView._43;

		// normalize
		(*vcDir).Normalize();
	}
}

/**
 * Cast given point from world space to screen space.
 * -> IN:     Vector4D		- vector to position in 3d space 
 * -> RETURN: SDimension2D  - corresponding point in 2d screen space
 */
SDimension2D COpenGLRenderDevice::Transform3Dto2D(const Vector4D &vcPoint)
{
	SDimension2D pt;
	pt.Width = pt.Height = 0;
	float fClip_x, fClip_y;
	float fXp, fYp, fWp;
	uint32 dwWidth, dwHeight;

	if( m_pCurrentCamera )
	{
		dwWidth  = m_ViewPort.Width;
		dwHeight = m_ViewPort.Height;

		fClip_x = (float)(dwWidth  >> 1);
		fClip_y = (float)(dwHeight >> 1);

		fXp =	(m_pCurrentCamera->m_mViewProjMatrix._11*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._21*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._31*vcPoint.z) + 
				(m_pCurrentCamera->m_mViewProjMatrix._41);
		fYp =	(m_pCurrentCamera->m_mViewProjMatrix._12*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._22*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._32*vcPoint.z) +
				(m_pCurrentCamera->m_mViewProjMatrix._42);
		fWp =	(m_pCurrentCamera->m_mViewProjMatrix._14*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._24*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._34*vcPoint.z) + 
				(m_pCurrentCamera->m_mViewProjMatrix._44);

		float fWpInv = 1.0f / fWp;

		// transform from [-1,1] to actual viewport dimensions
		pt.Width = (uint32)( (1.0f + (fXp * fWpInv)) * fClip_x );
		pt.Height = (uint32)( (1.0f + (fYp * fWpInv)) * fClip_y );
	}

	return pt;
}





void COpenGLRenderDevice::FlushRenderBatch()
{
	// Commit Dynamic Buffer
	GetVertexCacheManager()->ForcedCommitAll();

	getOpenGLMaterialRenderer()->QueueRenderBatch();

	// STEP 1 : Render water reflection
	if( CSGPWorldConfig::getInstance()->m_bShowWater )
	{
		if( m_pSceneFBO && GetWorldSystemManager()->needRenderWater() )
		{
			// switch Reflection map texture in FBO to color Attachment 0
			m_pSceneFBO->switchFramebufferTexture(0, m_pSceneFBO->getAuxiliaryTextureID());
			// Set viewport to quarter size
			glViewport(0, 0, getViewPort().Width/2, getViewPort().Height/2);

			// clear frame buffer
			m_pSceneFBO->clearOneDrawBuffers(0, m_ClearRedColor, m_ClearGreenColor, m_ClearBlueColor, m_ClearAlphaColor, true);

			// do draw Water Reflection Map
			m_RenderStage = SGPRS_WATERREFLECTION;
			getOpenGLWaterRenderer()->DoDrawWaterReflectionMap();

			// Restore viewport
			glViewport(0, 0, getViewPort().Width, getViewPort().Height);
			// switch back color Attachment 0 to Main Scene in FBO
			m_pSceneFBO->switchFramebufferTexture(0, m_pSceneFBO->getColorAttachmentsTextureID(0));
		
			m_RenderStage = SGPRS_NORMAL;
			// Clear Shader Program ID cache
			ISGPRenderBatch::m_ShaderProgramIdx = -1;
			COpenGLSLShaderProgram::m_CurrentProgramID = 0;
		}
	}

	// STEP 2 : Clear frame buffer
	if( m_pSceneFBO )
		m_pSceneFBO->clearOneDrawBuffers(0, m_ClearRedColor, m_ClearGreenColor, m_ClearBlueColor, m_ClearAlphaColor, false);

	// STEP 3 : Skydome
	if( CSGPWorldConfig::getInstance()->m_bShowSkyDome )
	{
		getOpenGLSkydomeRenderer()->DoDrawSkydomeRenderBatch();
	}

	// STEP 4 : Set Color Attachment1 for Depth texture
	if( m_pSceneFBO && GetWorldSystemManager()->isUsingDepthTexture() )
	{
		m_pSceneFBO->clearOneDrawBuffers(1, 1.0f, 1.0f, 1.0f, 1.0f, true);
		m_pSceneFBO->setDrawBuffersCount(2);
	}

	// STEP 5 : Terrain
	if( CSGPWorldConfig::getInstance()->m_bShowTerrain )
	{
		getOpenGLTerrainRenderer()->DoDrawTerrainRenderBatch();
	}

	// STEP 6 : Opaque Renderbatch
	if( CSGPWorldConfig::getInstance()->m_bShowStaticObject )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_Opaque();
	}

	// STEP 7 : Skin Animination
	if( CSGPWorldConfig::getInstance()->m_bShowSkeleton )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnim();
	}


	// Clear Shader Program ID cache
	ISGPRenderBatch::m_ShaderProgramIdx = -1;	
	COpenGLSLShaderProgram::m_CurrentProgramID = 0;

	// Color Attachment1 Depth texture Done!
	if( m_pSceneFBO )
		m_pSceneFBO->setDrawBuffersCount(1);

	// STEP 8 : water refraction and water render
	if( CSGPWorldConfig::getInstance()->m_bShowWater )
	{
		if( m_pSceneFBO && GetWorldSystemManager()->needRenderWater() )
		{
			getOpenGLWaterRenderer()->DoDrawWaterRefractionMap();

			// copy color attachment 0 ==> color attachment 2
			m_pSceneFBO->blitColorInFramebuffer(0, 2, 0, 0, getViewPort().Width, getViewPort().Height, 0, 0, getViewPort().Width, getViewPort().Height);


			getOpenGLWaterRenderer()->DoDrawWaterSimulation(m_pSceneFBO->getAuxiliaryTextureID(), m_pSceneFBO->getColorAttachmentsTextureID(2));
		}
	}
	getOpenGLTerrainRenderer()->AfterDrawTerrainRenderBatch();

	// STEP 9 : grass
	if( CSGPWorldConfig::getInstance()->m_bShowGrass )
	{
		getOpenGLGrassRenderer()->DoDrawGrassInstance();
	}


	// STEP 10 : Transparent Renderbatch
	if( CSGPWorldConfig::getInstance()->m_bShowStaticObject )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_Transparent();
	}


	// STEP 11 : SkinAnim with alpha Renderbatch
	if( CSGPWorldConfig::getInstance()->m_bShowSkeleton )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnimAlpha();
	}


	// STEP 12 : Particle
	if( CSGPWorldConfig::getInstance()->m_bShowParticle )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_Particle();
	}


	// STEP 13 : Post Process
	{
	}

	// STEP 14 : Debug info
	if( CSGPWorldConfig::getInstance()->m_bShowDebugLine )
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_DebugLine();
	}

	getOpenGLMaterialRenderer()->AfterDrawRenderBatch();	

	// Clear Dynamic Buffer
	GetVertexCacheManager()->ForcedClearAll();

	// Clear Shader Program ID cache
	ISGPRenderBatch::m_ShaderProgramIdx = -1;
	COpenGLSLShaderProgram::m_CurrentProgramID = 0;
}

void COpenGLRenderDevice::FlushEditorLinesRenderBatch(bool bNoDepthLine, float LineWidth)
{
	// Commit Dynamic Buffer
	GetVertexCacheManager()->ForcedCommitAll();

	getOpenGLMaterialRenderer()->QueueRenderBatch();


	if( LineWidth != 1.0f )
		glLineWidth( LineWidth );

	if( bNoDepthLine )
	{
		// Editor Debug Info Line without depth
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_NoDepthline();
	}
	else
	{
		getOpenGLMaterialRenderer()->DoDrawRenderBatch_DebugLine();
	}

	if( LineWidth != 1.0f )
		glLineWidth( 1.0f );

	getOpenGLMaterialRenderer()->AfterDrawRenderBatch();

	// Clear Dynamic Buffer
	GetVertexCacheManager()->ForcedClearAll();

	// Clear Shader Program ID cache
	ISGPRenderBatch::m_ShaderProgramIdx = -1;
	COpenGLSLShaderProgram::m_CurrentProgramID = 0;
}



SGP_TTFFont* COpenGLRenderDevice::CreateTTFFont(FT_Library& FTLib)
{
	return new SGP_OpenGLTTFFont(FTLib, this);
}

void COpenGLRenderDevice::BeginRenderText()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &FontMaterial_info = Mat_List[ISGPMaterialSystem::eMaterial_font];
	getOpenGLMaterialRenderer()->PushMaterial(FontMaterial_info.m_material, MM_Add);
	getOpenGLMaterialRenderer()->ComputeMaterialPass();
	getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);
}

void COpenGLRenderDevice::EndRenderText()
{
	m_pFontManager->FlushAllFonts();

	getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	getOpenGLMaterialRenderer()->PopMaterial();
}

void COpenGLRenderDevice::SetActiveFont(const char* FontName)
{
	m_pFontManager->SetActiveFont( FontName );
}

bool COpenGLRenderDevice::CreateFontInManager(const char* FontName, const String& FontFilePath, bool bBold, bool bItalic, uint16 iSize)
{
	String FontAbsolutePath = FontFilePath;
	// Identify font file by their absolute filenames if possible.
	if( !File::isAbsolutePath(FontAbsolutePath) )
	{
		FontAbsolutePath = m_WorkingDir + File::separatorString + FontFilePath;
	}

	if( m_pFontManager->IsFontLoaded(FontName) )
		return true;

	if( !m_pFontManager->AddFont( FontName, FontAbsolutePath, iSize, bBold, bItalic ) )
		return false;

	m_pFontManager->SetActiveFont( FontName );

	return true;
}

void COpenGLRenderDevice::DrawTextInPos(int xPos, int yPos, uint32 flag, float RealFontSize, uint8 r, uint8 g, uint8 b, wchar_t* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	int Length = vswprintf(0, 0, format, ArgPtr)/*_vscwprintf(format, ArgPtr)*/ + 1;
	if( Length > MAX_FONT_STRING )
	{
		m_pLogger->writeToLog(String("Render text buffer overflow!"), ELL_ERROR);
		return;
	}
	vswprintf(m_FontString, Length, format, ArgPtr);
	va_end(ArgPtr);

	Colour FontColor( r, g, b );
	float screenPos_x = static_cast<float>(xPos);
	float screenPos_y = static_cast<float>(yPos);

	m_pFontManager->DoDrawTextInPos(String(m_FontString), screenPos_x, screenPos_y, FontColor, flag, RealFontSize, false, (flag & SGPFDL_UNDERLINE) ?  true : false);

}

void COpenGLRenderDevice::DrawTextAlignWrap(int xPos, int yPos, uint32 flag, float RealFontSize, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align, uint8 r, uint8 g, uint8 b, wchar_t* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	int Length = vswprintf(0, 0, format, ArgPtr)/*_vscwprintf(format, ArgPtr)*/ + 1;
	if( Length > MAX_FONT_STRING )
	{
		m_pLogger->writeToLog(String("Render text buffer overflow!"), ELL_ERROR);
		return;
	}
	vswprintf(m_FontString, Length, format, ArgPtr);
	va_end(ArgPtr);

	Colour FontColor( r, g, b );
	float screenPos_x = static_cast<float>(xPos);
	float screenPos_y = static_cast<float>(yPos);

	m_pFontManager->DoDrawTextAlignWrap(String(m_FontString), screenPos_x, screenPos_y, FontColor, flag, RealFontSize, false, ((flag & SGPFDL_UNDERLINE) ?  true : false), xPosMax, xOffset, lineSpacing, align);

}

void COpenGLRenderDevice::PreCacheChar(const String& sText)
{
	m_pFontManager->PreCacheChar(sText);
}

void COpenGLRenderDevice::setWorkingDirection(const String& workingDir)
{
	m_WorkingDir = workingDir;
	if( m_pTextureManager )
		m_pTextureManager->setWorkingDirection(m_WorkingDir);
	if( m_pModelManager )
		m_pModelManager->setWorkingDirection(m_WorkingDir);
	if( m_pParticleManager )
		m_pParticleManager->setWorkingDirection(m_WorkingDir);
	if( m_pEffectSystemManager )
		m_pEffectSystemManager->setWorkingDirection(m_WorkingDir);
}







ISGPTexture* COpenGLRenderDevice::createTexture( ISGPImage* pImage, 
	const String& AbsolutePath, bool bGenMipMap )
{
	return new COpenGLTexture(pImage, AbsolutePath, this, bGenMipMap);
}

AbstractMaterial* COpenGLRenderDevice::createMaterial()
{
	return new OpenGLMaterial();
}

AbstractMaterial* COpenGLRenderDevice::createMaterial(char* MaterialStr)
{
	return new OpenGLMaterial(MaterialStr);
}


// Creates one new ISGPParticleSystem from Particle setting
uint32 COpenGLRenderDevice::createOpenGLParticleSystem(const SGPMF1ParticleTag& PartSetting)
{	
	Matrix4x4 ParticleWorldMatrix;
	ParticleWorldMatrix.Identity();
	uint32 ParticleID = GetParticleManager()->createParticleSystem(ParticleWorldMatrix);
	ISGPParticleSystem* pSystem = GetParticleManager()->getParticleSystemByID( ParticleID );
	
	SPARK::Group** pParticleGroups = pSystem->createParticleGroups(PartSetting);
	if( pParticleGroups )
	{
		for(uint32 i=0; i<PartSetting.m_SystemParam.m_groupCount; ++i)
			pSystem->addGroup( pParticleGroups[i] );

		delete [] pParticleGroups;
		pParticleGroups = NULL;
	}
	pSystem->enableAABBComputing(PartSetting.m_SystemParam.m_bEnableAABBCompute);

	return ParticleID;
}

// Creates one new OpenGL Particle Renderer from ParticleRenderParam setting
SPARK::Renderer* COpenGLRenderDevice::createOpenGLParticleRenderer( const ParticleRenderParam& renderParam )
{
	switch(renderParam.m_type)
	{
	case Render_Quad:
		{
			const ParticleQuadRenderData& quadData = renderParam.m_quadData;
			SPARKOpenGLQuadRenderer* quadRender = SPARKOpenGLQuadRenderer::create( this, quadData.m_xScale, quadData.m_yScale );
			quadRender->setTexture( GetTextureManager()->getTextureIDByName(String(quadData.m_texPath)) );
			quadRender->setTexturingMode( (SPARK::TexturingMode)quadData.m_texMode );
			quadRender->setAtlasDimensions( quadData.m_xDimension, quadData.m_yDimension );
			quadRender->setOrientation( (SPARK::OrientationPreset)quadData.m_nOrientation );
			if(quadData.m_nOrientation == SPARK::AROUND_AXIS)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
			}
			else if(quadData.m_nOrientation == SPARK::TOWARDS_POINT)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
			}
			else if(quadData.m_nOrientation == SPARK::FIXED_ORIENTATION)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
				quadRender->upVector.x = quadData.m_upVector[0];
				quadRender->upVector.y = quadData.m_upVector[1];
				quadRender->upVector.z = quadData.m_upVector[2];
			}
			return quadRender;
		}
		break;
	case Render_Line:
		{
			const ParticleLineRenderData& lineData = renderParam.m_lineData;
			SPARKOpenGLLineRenderer* lineRender = SPARKOpenGLLineRenderer::create( this, lineData.m_length, lineData.m_width );
			return lineRender;
		}
		break;
	case Render_Point:
		{
			const ParticlePointRenderData& pointData = renderParam.m_pointData;
			SPARKOpenGLPointRenderer* pointRender = SPARKOpenGLPointRenderer::create( this, pointData.m_size );
			pointRender->setType( (SPARK::PointType)pointData.m_type );
			pointRender->setTexture( GetTextureManager()->getTextureIDByName(String(pointData.m_texPath)) );
			return pointRender;
		}
		break;
	}
	return NULL;
}


//! Returns an image created from the last rendered frame.
ISGPImage* COpenGLRenderDevice::createScreenShot(SGP_PIXEL_FORMAT format)
{

	// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
	if( GLEE_MESA_pack_invert )
		glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

	if (format == SGPPF_UNKNOWN)
		format = getPixelFormat();

	GLenum fmt;
	GLenum type;
	switch (format)
	{
	case SGPPF_A1R5G5B5:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case SGPPF_R5G6B5:
		fmt = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case SGPPF_R8G8B8:
		fmt = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case SGPPF_A8R8G8B8:
		fmt = GL_BGRA;
		if (Version > 110)
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
		else
			type = GL_UNSIGNED_BYTE;
		break;
	case SGPPF_A4R4G4B4:
		fmt = GL_BGRA;
		if (Version > 110)
			type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
		else
			type = GL_UNSIGNED_SHORT;
		break;
	case SGPPF_R16F:
		if (GLEE_ARB_texture_rg)
			fmt = GL_RED;
		else
			fmt = GL_LUMINANCE;
#ifdef GL_ARB_half_float_pixel
		if (GLEE_ARB_half_float_pixel)
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = SGPPF_R32F;
		}
		break;
	case SGPPF_G16R16F:
#ifdef GL_ARB_texture_rg
		if (GLEE_ARB_texture_rg)
			fmt = GL_RG;
		else
#endif
			fmt = GL_LUMINANCE_ALPHA;
#ifdef GL_ARB_half_float_pixel
		if (GLEE_ARB_half_float_pixel)
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = SGPPF_G32R32F;
		}
		break;
	case SGPPF_A16B16G16R16F:
		fmt = GL_BGRA;
#ifdef GL_ARB_half_float_pixel
		if (GLEE_ARB_half_float_pixel)
			type = GL_HALF_FLOAT_ARB;
		else
#endif
		{
			type = GL_FLOAT;
			format = SGPPF_A32B32G32R32F;
		}
		break;
	case SGPPF_R32F:
		if (GLEE_ARB_texture_rg)
			fmt = GL_RED;
		else
			fmt = GL_LUMINANCE;
		type = GL_FLOAT;
		break;
	case SGPPF_G32R32F:
#ifdef GL_ARB_texture_rg
		if (GLEE_ARB_texture_rg)
			fmt = GL_RG;
		else
#endif
			fmt = GL_LUMINANCE_ALPHA;
		type = GL_FLOAT;
		break;
	case SGPPF_A32B32G32R32F:
		fmt = GL_BGRA;
		type = GL_FLOAT;
		break;
	default:
		fmt = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	}

	ISGPImage* newImage = GetTextureManager()->createImage(format, m_ScreenSize);

	uint8* pixels = 0;
	if (newImage)
		pixels = static_cast<uint8*>(newImage->lock());
	if (pixels)
	{
		GLenum tgt=GL_FRONT;

		glReadBuffer(tgt);
		glReadPixels(0, 0, m_ScreenSize.Width, m_ScreenSize.Height, fmt, type, pixels);
		testGLError();
		glReadBuffer(GL_BACK);
	}

#ifdef GL_MESA_pack_invert
	if (GLEE_MESA_pack_invert)
		glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
	else
#endif
	if (pixels && newImage)
	{
		// opengl images are horizontally flipped, so we have to fix that here.
		const int32 pitch = newImage->getPitch();
		uint8* p2 = pixels + (m_ScreenSize.Height - 1) * pitch;
		uint8* tmpBuffer = new uint8[pitch];
		for (uint32 i=0; i < m_ScreenSize.Height; i += 2)
		{
			memcpy(tmpBuffer, pixels, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				pixels[j]=(u8)(p2[j]*255.f);
//			}
			memcpy(pixels, p2, pitch);
//			for (u32 j=0; j<pitch; ++j)
//			{
//				p2[j]=(u8)(tmpBuffer[j]*255.f);
//			}
			memcpy(p2, tmpBuffer, pitch);
			pixels += pitch;
			p2 -= pitch;
		}
		delete [] tmpBuffer;
		tmpBuffer = NULL;
	}

	if (newImage)
	{
		newImage->unlock();
		if (testGLError() || !pixels)
		{
			delete newImage;
			newImage = NULL;
			return NULL;
		}
	}
	return newImage;
}


//! Convert SGP_PRIMITIVE_TYPE to OpenGL equivalent
GLenum COpenGLRenderDevice::primitiveTypeToGL(SGP_PRIMITIVE_TYPE type) const
{
	switch (type)
	{
		case SGPPT_POINTS:
			return GL_POINTS;
		case SGPPT_LINE_STRIP:
			return GL_LINE_STRIP;
		case SGPPT_LINE_LOOP:
			return GL_LINE_LOOP;
		case SGPPT_LINES:
			return GL_LINES;
		case SGPPT_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case SGPPT_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		case SGPPT_TRIANGLES:
			return GL_TRIANGLES;
		case SGPPT_POINT_SPRITES:
//#ifdef GL_ARB_point_sprite
//			return GL_POINT_SPRITE_ARB;
//#else
			return GL_POINTS;
//#endif
	}
	return GL_TRIANGLES;
}

GLenum COpenGLRenderDevice::getZBufferBits() const
{
	GLenum bits = 0;
	switch (m_Params.ZBufferBits)
	{
	case 16:
		bits = GL_DEPTH_COMPONENT16;
		break;
	case 24:
		bits = GL_DEPTH_COMPONENT24;
		break;
	case 32:
		bits = GL_DEPTH_COMPONENT32;
		break;
	default:
		bits = GL_DEPTH_COMPONENT;
		break;
	}
	return bits;
}

//! prints error if an error happened.
bool COpenGLRenderDevice::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch (g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		m_pLogger->writeToLog(String("GL_INVALID_ENUM"), ELL_ERROR); break;
	case GL_INVALID_VALUE:
		m_pLogger->writeToLog(String("GL_INVALID_VALUE"), ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		m_pLogger->writeToLog(String("GL_INVALID_OPERATION"), ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		m_pLogger->writeToLog(String("GL_STACK_OVERFLOW"), ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		m_pLogger->writeToLog(String("GL_STACK_UNDERFLOW"), ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		m_pLogger->writeToLog(String("GL_OUT_OF_MEMORY"), ELL_ERROR); break;
	case GL_TABLE_TOO_LARGE:
		m_pLogger->writeToLog(String("GL_TABLE_TOO_LARGE"), ELL_ERROR); break;
#if defined(GL_EXT_framebuffer_object)
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		m_pLogger->writeToLog(String("GL_INVALID_FRAMEBUFFER_OPERATION"), ELL_ERROR); break;
#endif
	};
	jassertfalse;
	return true;
#else
	return false;
#endif
}