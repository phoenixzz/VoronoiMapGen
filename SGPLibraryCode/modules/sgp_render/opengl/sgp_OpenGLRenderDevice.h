

#ifndef __SGP_OPENGLRENDERER_HEADER__
#define __SGP_OPENGLRENDERER_HEADER__

#include "../../sgp_enginedevice/enginedevice/sgp_CreationParameter.h"

#if SGP_WINDOWS
class SGPDeviceWin32;
#elif SGP_LINUX
class SGPDeviceLinux;
#elif SGP_MAC
class SGPDeviceMacOSX;
#elif SGP_IOS
class SGPDeviceIOS;
#elif SGP_ANDROID
class SGPDeviceANDROID;
#endif

//==============================================================================
/**
    A base class that should be implemented by classes which want to render openGL
    on a background thread.
*/
class SGP_API COpenGLRenderDevice : public ISGPRenderDevice, public COpenGLExtensionHandler
{
public:
	#if SGP_WINDOWS
		COpenGLRenderDevice(const SGPCreationParameters& params, SGPDeviceWin32* device);
		//! inits the windows specific parts of the open gl driver
		bool initDriver(SGPDeviceWin32* device);
		bool changeRenderContext(const HDC _HDc, const HGLRC _HRc, const HWND _Window, SGPDeviceWin32* device);
	#elif SGP_LINUX
		COpenGLRenderDevice(const SGPCreationParameters& params, SGPDeviceLinux* device);
		//! inits the GLX specific parts of the open gl driver
		bool initDriver(SGPDeviceLinux* device);
		bool changeRenderContext(const SExposedVideoData& videoData, SGPDeviceLinux* device);

	#elif SGP_MAC
		COpenGLRenderDevice(const SGPCreationParameters& params, SGPDeviceMacOSX *device);
	#elif SGP_IOS
	#elif SGP_ANDROID	
	#endif

    virtual ~COpenGLRenderDevice();

	// Manager
	virtual CSGPResourceLoaderMuitiThread* GetMTResourceLoader() { return m_pMTResourceLoader; }
	virtual CSGPTextureManager* GetTextureManager()	{ return m_pTextureManager; }
	virtual ISGPShaderManager* GetShaderManager() { return m_pShaderManager; }
	virtual ISGPMaterialSystem* GetMaterialSystem() { return m_pMaterialSystem; }
	virtual ISGPVertexCacheManager* GetVertexCacheManager() { return m_pVertexCacheManager; }
	virtual ISGPModelManager* GetModelManager() { return m_pModelManager; }
	virtual ISGPParticleManager* GetParticleManager() { return m_pParticleManager; }
	virtual ISGPEffectSystemManager* GetEffectInstanceManager() { return m_pEffectSystemManager; }
	virtual ISGPWorldSystemManager* GetWorldSystemManager() { return m_pWorldSystemManager; }
	virtual ISGPFontManager* GetFontManager() { return m_pFontManager; }
	virtual ISGPInstanceManager* GetInstanceManager() { return m_pInstanceManager; }

	//! clears back buffer
	virtual bool beginScene(	bool bClearColorBuffer = true, 
								bool bClearDepthBuffer = true,
								bool bClearStencilBuffer = true );

	//! presents the rendered scene on the screen, returns false if failed
	virtual bool endScene();

	//! Render something in Render Batch
	virtual void FlushRenderBatch();

	//! create other frame buffer
	virtual void createRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer=false);
	virtual void recreateRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer=false);
	virtual void deleteRenderToFrameBuffer();

	//! set other frame buffer for below rendering
	virtual void setRenderToFrameBuffer();

	//! reset to main back buffer and BitBlit color from other frame buffer
	virtual void renderBackToMainBuffer();


	virtual void clearZBuffer();

	virtual void setClearColor(float fRed, float fGreen, float fBlue, float fAlpha);

	//! queries the features of the driver, returns true if feature is available
	virtual bool queryDriverFeature(SGP_DRIVER_FEATURE feature) const
	{
		return m_pOpenGLConfig->m_bFeatureEnabled[feature] && COpenGLExtensionHandler::queryDriverFeature(feature);
	}
	//! Disable a feature of the driver.
	virtual void setDriverFeature(SGP_DRIVER_FEATURE feature, bool flag)
	{
		m_pOpenGLConfig->m_bFeatureEnabled[feature] = flag;
	}

	//! Check if the driver was recently reset.
	/** Only valid For d3d devices.
	*/
	virtual bool checkDriverReset() { return false; }
		
	//! Check if the Resource Loading (hard disk files) was loading using Multi-thread.
	virtual bool isResLoadingMultiThread() { return m_Params.MultiThreadResLoading; }

	//! Check Is the viewport in portrait or landscape mode ( only valid in opengl es)
	virtual bool isViewportRotated() { return false; }

	//! Only used by the internal engine. Used to notify the driver that
	//! the window was resized.
	virtual void onResize(const uint32 width, const uint32 height);

	//! get color format of the current color buffer
	virtual SGP_PIXEL_FORMAT getPixelFormat() const
	{
		return m_PixelFormat;
	}

	//! Get the current render stage
	virtual SGP_RENDER_STAGE getCurrentRenderStage() const
	{
		return m_RenderStage;
	}

	//! get screen size
	virtual const SDimension2D& getScreenSize() const
	{
		return m_ScreenSize;
	}

	// returns the current size of the screen or rendertarget
	virtual const SDimension2D& getCurrentRenderTargetSize() const;

	//! Returns an image created from the last rendered frame.
	virtual ISGPImage* createScreenShot(SGP_PIXEL_FORMAT format=SGPPF_UNKNOWN);

	//! Returns an API texture created from ISGPImage class.
	virtual ISGPTexture* createTexture( ISGPImage* pImage, 
			const String& AbsolutePath,
			bool bGenMipMap = false );

	//! Create a OpenGL Material according to material string
	virtual AbstractMaterial* createMaterial();
	virtual AbstractMaterial* createMaterial(char* MaterialStr);

	// Creates one new ISGPParticleSystem from Particle setting
	virtual uint32 createOpenGLParticleSystem(const SGPMF1ParticleTag& PartSetting);
	// Creates one new OpenGL Particle Renderer from ParticleRenderParam setting
	virtual SPARK::Renderer* createOpenGLParticleRenderer( const ParticleRenderParam& renderParam );


	// CAMERA param
	virtual void setCameraMode(SGP_CAMERAMODE_TYPE Mode);
	virtual void setFov(float fov);
	virtual void setViewPort(const SViewPort& area);
	virtual const SViewPort& getViewPort() const
	{
		return m_ViewPort;
	}
	virtual void setNearFarClipPlane(float fNear, float fFar);
	virtual void setProjMatrixParams(float fAspect);
	virtual void setViewMatrix3D( const Vector4D& vcRight, const Vector4D& vcUp, 
								  const Vector4D& vcDir, const Vector4D& vcEyePos );
	virtual void setViewMatrixLookAt( const Vector4D& vcPos, const Vector4D& vcPoint, 
									  const Vector4D& vcWorldUp);

	virtual void getCamreaPosition(Vector4D* pCameraPos);
	virtual void getCamreaViewDirection(Vector3D* pViewDir);
	virtual void getViewFrustrum(Plane *pPlane);
	virtual void getViewMatrix(Matrix4x4& mat);
	virtual void getProjMatrix(Matrix4x4& mat);
	virtual void getViewProjMatrix(Matrix4x4& mat);

	virtual void Transform2Dto3D(const SDimension2D &pt, Vector4D *vcOrig, Vector4D *vcDir);
	virtual SDimension2D Transform3Dto2D(const Vector4D &vcPoint);





	virtual SGP_DRIVER_TYPE getVideoDriverType() const
	{
		return SGPDT_OPENGL;
	}
	virtual const String getVideoDriverName() const
	{
		return m_VideoDriverName;
	}
	virtual String getVendorName() 
	{ 
		return m_VendorName; 
	}

	virtual void setAmbientLight(const Colour& color)
	{
		m_pOpenGLConfig->m_DefaultAmbientColor = color;
	}

	virtual void setWorkingDirection(const String& workingDir);

	virtual void setRenderDeviceTime(uint32 devicetime, double deltatime)
	{
		m_RenderDeviceTime = devicetime;
		m_RenderDeviceDelta = deltatime;
	}
	virtual uint32 getRenderDeviceTime()
	{
		return m_RenderDeviceTime;
	}
	virtual double getDeltaTime()
	{
		return m_RenderDeviceDelta;
	}


	virtual void FlushEditorLinesRenderBatch(bool bNoDepthLine = false, float LineWidth = 1.0f );

	virtual SGP_TTFFont* CreateTTFFont(FT_Library& FTLib);
	virtual void BeginRenderText();
	virtual void EndRenderText();
	virtual void SetActiveFont(const char* FontName);
	virtual bool CreateFontInManager(const char* FontName, const String& FontFilePath, bool bBold, bool bItalic, uint16 iSize);
	virtual void DrawTextInPos(int xPos, int yPos, uint32 flag, float RealFontSize, uint8 r, uint8 g, uint8 b, wchar_t* format, ...);
	virtual void DrawTextAlignWrap(int xPos, int yPos, uint32 flag, float RealFontSize, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align, uint8 r, uint8 g, uint8 b, wchar_t* format, ...);

	virtual void PreCacheChar(const String& sText);


public:
	//! checks if an OpenGL error has happend and prints it
	//! for performance reasons only available in debug mode
	bool testGLError();

	//! Get ZBuffer bits.
	GLenum getZBufferBits() const;

	//! Convert SGP_PRIMITIVE_TYPE to OpenGL equivalent
	GLenum primitiveTypeToGL(SGP_PRIMITIVE_TYPE type) const;

	//! Get COpenGLCamera class & COpenGLMaterialRenderer & COpenGLTerrainRenderer & COpenGLSkydomeRenderer & COpenGLWaterRenderer class
	COpenGLCamera* getOpenGLCamera() { return m_pCurrentCamera; }
	COpenGLMaterialRenderer* getOpenGLMaterialRenderer() { return m_pMaterialRenderer; }
	COpenGLTerrainRenderer* getOpenGLTerrainRenderer() { return m_pTerrainRenderer; }
	COpenGLSkydomeRenderer* getOpenGLSkydomeRenderer() { return m_pSkydomeRenderer; }
	COpenGLWaterRenderer* getOpenGLWaterRenderer() { return m_pWaterRenderer; }
	COpenGLGrassRenderer* getOpenGLGrassRenderer() { return m_pGrassRenderer; }

private:
	COpenGLRenderDevice();

	//! inits the parts of the open gl driver used on all platforms
	bool OnDriverInit();

	//!  inits the render state of OPENGL device
	void InitOpenGLRenderState();


	SGP_DECLARE_NON_COPYABLE (COpenGLRenderDevice)

private:
	static const int		MAX_FONT_STRING	= 2048;

	COpenGLCamera*			m_pCurrentCamera;
	Logger*					m_pLogger;
	COpenGLMaterialRenderer*m_pMaterialRenderer;
	COpenGLTerrainRenderer* m_pTerrainRenderer;
	COpenGLSkydomeRenderer* m_pSkydomeRenderer;
	COpenGLWaterRenderer*   m_pWaterRenderer;
	COpenGLGrassRenderer*	m_pGrassRenderer;
	COpenGLConfig*			m_pOpenGLConfig;
	SDimension2D			m_ScreenSize;
	SDimension2D			m_CurrentRTSize;
	SGP_PIXEL_FORMAT		m_PixelFormat;
	SGP_RENDER_STAGE		m_RenderStage;
	SViewPort				m_ViewPort;
	String					m_VideoDriverName;
	String					m_VendorName;

	String					m_WorkingDir;

	uint32					m_RenderDeviceTime;		// in milliseconds
	double					m_RenderDeviceDelta;	// in milliseconds


	float					m_ClearRedColor, m_ClearGreenColor, m_ClearBlueColor, m_ClearAlphaColor;
	uint32					m_AntiAlias;
	SGPCreationParameters	m_Params;
	wchar_t					m_FontString[MAX_FONT_STRING];

	// Manager
	CSGPTextureManager*		m_pTextureManager;
	ISGPShaderManager*		m_pShaderManager;
	ISGPMaterialSystem*		m_pMaterialSystem;
	ISGPVertexCacheManager* m_pVertexCacheManager;
	ISGPModelManager*		m_pModelManager;
	ISGPParticleManager*	m_pParticleManager;
	ISGPEffectSystemManager*m_pEffectSystemManager;
	ISGPWorldSystemManager* m_pWorldSystemManager;
	ISGPFontManager*		m_pFontManager;
	ISGPInstanceManager*	m_pInstanceManager;

	// MultiThread Loader
	CSGPResourceLoaderMuitiThread*	m_pMTResourceLoader;

	// Render to Frame Buffer Object(FBO)
	COpenGLFrameBufferObject*		m_pSceneFBO;	

	#if SGP_WINDOWS
		HDC HDc, BackUpHDc; // Private GDI Device Context
		HGLRC HRc, BackUpHRc;
		HWND Window, BackUpWindow;		
		SGPDeviceWin32 *Win32Device;
	#elif SGP_LINUX
		GLXDrawable Drawable;
		Display* X11Display;
		SGPDeviceLinux *X11Device;
	#elif SGP_MAC
		SGPDeviceMacOSX *OSXDevice;
	#elif SGP_IOS
	#elif SGP_ANDROID
	#endif
};


#endif   // __SGP_OPENGLRENDERER_HEADER__
