#ifndef __SGP_RENDERDEVICE_HEADER__
#define __SGP_RENDERDEVICE_HEADER__

	//! Interface to driver which is able to perform 2d and 3d graphics functions.
	/** This interface is one of the most important interfaces of
	the SGP Engine: All rendering and texture manipulation is done with
	this interface. 
	*/
	class ISGPRenderDevice
	{
	public:
		// Manager:
		// Multi-Thread Resource Loader Manager
		virtual CSGPResourceLoaderMuitiThread* GetMTResourceLoader() = 0;
		// Texture Manager
		virtual CSGPTextureManager* GetTextureManager() = 0;
		// Material System
		virtual ISGPMaterialSystem* GetMaterialSystem() = 0;
		// Shader Manager
		virtual ISGPShaderManager* GetShaderManager() = 0;
		// VertexCache Manager
		virtual ISGPVertexCacheManager* GetVertexCacheManager() = 0;
		// Model Manager
		virtual ISGPModelManager* GetModelManager() = 0;
		// Camera Manager

		// Particle Manager
		virtual ISGPParticleManager* GetParticleManager() = 0;
		// Effect Instance Manager
		virtual ISGPEffectSystemManager* GetEffectInstanceManager() = 0;

		// World Manager
		virtual ISGPWorldSystemManager* GetWorldSystemManager() = 0;

		// Font Manager
		virtual ISGPFontManager* GetFontManager() = 0;
		// Log Manager

		// Instance Manager
		virtual ISGPInstanceManager* GetInstanceManager() = 0;
		
		virtual ~ISGPRenderDevice() {}

		//! Applications must call this method before performing any rendering.
		/** This method can clear the color- buffer, the z-buffer and stencil buffer.
		\param bClearColorBuffer Specifies if the color buffer should be
		cleared, which means that the screen is filled with the color
		specified. If this parameter is false, the back buffer will	not be cleared.
		\param bClearDepthBuffer Specifies if the depth buffer (z buffer) should
		be cleared. It is not nesesarry to do so if only 2d drawing is used.
		\param bClearStencilBuffer Specifies if the stencil buffer should
		be cleared. It is not nesesarry to do so if only 2d drawing is used.
		\return False if failed. */
		virtual bool beginScene( bool bClearColorBuffer = true,
								 bool bClearDepthBuffer = true,
								 bool bClearStencilBuffer = true ) = 0;

		/** This method can be used to set clear color of back-buffer
		It must be called before beginScene
		*/
		virtual void setClearColor(float fRed, float fGreen, float fBlue, float fAlpha) = 0;

		//! Clears the ZBuffer.
		/** Note that you usually need not to call this method, as it
		is automatically done in ISGPRenderDevice::beginScene() if you 
		enable zBuffer.	But if you have to render some special things, 
		you can clear the zbuffer during the rendering process with
		this method any time.
		*/
		virtual void clearZBuffer() = 0;

		//! Presents the rendered image to the screen.
		/** Applications must call this method after performing any	rendering.
		\return False if failed and true if succeeded. */
		virtual bool endScene() = 0;

		//! Actually render the rendered scene to color buffer.
		virtual void FlushRenderBatch() = 0;


		//! Create Render Target to other Frame Buffer
		virtual void createRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer=false) = 0;
		virtual void recreateRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer=false) = 0;
		virtual void deleteRenderToFrameBuffer() = 0;

		//! Set Render Target to other Frame Buffer, performing any rendering after this call will
		//	be rendered in this render target
		virtual void setRenderToFrameBuffer() = 0;

		//! Reset Render Target to Main Back buffer and BitBlit render target color to main backbuffer
		virtual void renderBackToMainBuffer() = 0;


		//! Queries the features of the driver.
		/** Returns true if a feature is available
		\param feature Feature to query.
		\return True if the feature is available, false if not. */
		virtual bool queryDriverFeature(SGP_DRIVER_FEATURE feature) const = 0;

		//! set a feature of the driver.
		/** It is not possible to enable unsupported features this way, though.
		\param feature Feature to set.
		\param flag */
		virtual void setDriverFeature(SGP_DRIVER_FEATURE feature, bool flag) = 0;

		//! Check if the driver was recently reset.
		/** For d3d devices you will need to recreate the RTTs if the
		driver was reset. Should be queried right after beginScene().
		*/
		virtual bool checkDriverReset() = 0;

		//! Check if the Resource Loading (hard disk files) was loading using Multi-thread.
		virtual bool isResLoadingMultiThread() = 0;

		//! Check Is the viewport in portrait or landscape mode ( only valid in opengl es)
		virtual bool isViewportRotated() = 0;

		//! Event handler for resize events. Only used by the engine internally.
		/** Used to notify the driver that the window was resized.
		Usually, there is no need to call this method. */
		virtual void onResize(const uint32 width, const uint32 height) = 0;

		//! Get the current color format of the color buffer
		/** \return Color format of the color buffer. */
		virtual SGP_PIXEL_FORMAT getPixelFormat() const = 0;

		//! Get the current render stage
		/** \return the current render stage. */
		virtual SGP_RENDER_STAGE getCurrentRenderStage() const = 0;

		//! Get the size of the screen or render window.
		/** \return Size of screen or render window. */
		virtual const SDimension2D& getScreenSize() const = 0;

		//! Get the size of the current render target
		/** This method will return the screen size if the driver
		doesn't support render to texture, or if the current render
		target is the screen.
		\return Size of render target or screen/window */
		virtual const SDimension2D& getCurrentRenderTargetSize() const = 0;

		//! Make a screenshot of the last rendered frame.
		/** \return An image created from the last rendered frame. */
		virtual ISGPImage* createScreenShot(SGP_PIXEL_FORMAT format=SGPPF_UNKNOWN) = 0;

		//! Create a OpenGL/D3D texture according to ISGPImage pointer
		/** \param pImage  ISGPImage class pointer
		\param AbsolutePath texture name for Hash Table
		\param bGenMipMap texture auto gen mipmap if texture has no mipmap in it
		\return An Video API Texture created from ISGPImage */
		virtual ISGPTexture* createTexture( ISGPImage* pImage, 
			const String& AbsolutePath,
			bool bGenMipMap = false ) = 0;

		//! Create a OpenGL/D3D Material according to material string
		// creates a default material containing 1 empty pass
		virtual AbstractMaterial* createMaterial() = 0;
		/*\param  MaterialStr the material string */ 
		virtual AbstractMaterial* createMaterial(char* MaterialStr) = 0;

		// Creates one new ISGPParticleSystem from Particle setting
		virtual uint32 createOpenGLParticleSystem(const SGPMF1ParticleTag& PartSetting) = 0;
		// Creates one new OpenGL Particle Renderer from ParticleRenderParam setting
		virtual SPARK::Renderer* createOpenGLParticleRenderer( const ParticleRenderParam& renderParam ) = 0;


		// CAMERA STUFF:
		// set mode for current camera, 0:=3D(perspective), 1:=3D(orthogonal) 2:=2D
		virtual void setCameraMode(SGP_CAMERAMODE_TYPE Mode) = 0;
		// set perspective field of horizontal view (in degrees)
		virtual void setFov(float fov) = 0;
		//! Sets a new viewport.
		/** Every rendering operation is done into this new area.
		\param area: Rectangle defining the new area of rendering
		operations. */
		virtual void setViewPort(const SViewPort& area) = 0;

		//! Gets the area of the current viewport.
		virtual const SViewPort& getViewPort() const = 0;

		//! Set near and Far clipping plane for Projection.
		/** 
		\param fNear The near plane. 
		\param fFar The far plane.	*/
		virtual void setNearFarClipPlane(float fNear, float fFar) = 0;

		/** set perspective Aspect for current camera
		to Build Project Matrix.
		\param fAspect	. 
		*/
		virtual void setProjMatrixParams(float fAspect) = 0;

		// set current view matrix from cam's vRight, vUp, vDir, vPos (must be normalized vector)
		virtual void setViewMatrix3D( const Vector4D& vcRight, const Vector4D& vcUp, 
									  const Vector4D& vcDir, const Vector4D& vcEyePos ) = 0;

		// set current view matrix: EyePos, Lookat, world up (could be un-normalized vector)
		virtual void setViewMatrixLookAt( const Vector4D& vcPos, const Vector4D& vcPoint, 
										  const Vector4D& vcWorldUp) = 0;

		// ! Gets camera position
		virtual void getCamreaPosition(Vector4D* pCameraPos) = 0;
		// ! Gets camera view direction vector
		virtual void getCamreaViewDirection(Vector3D* pViewDir) = 0;

		//! Get view Frustrum planes
		// There are 6 clipping planes available 
		virtual void getViewFrustrum(Plane *pPlane) = 0;

		//! Gets View / Projection matrices.
		/** \param mat view Matrix describing the transformation. */
		virtual void getViewMatrix(Matrix4x4& mat) = 0;
		virtual void getProjMatrix(Matrix4x4& mat) = 0;
		virtual void getViewProjMatrix(Matrix4x4& mat) = 0;

		// screen to worldray, give 2 vectors for output
		virtual void Transform2Dto3D(const SDimension2D &pt, Vector4D *vcOrig, Vector4D *vcDir) = 0;

		// cast world position to screen coordinates
		virtual SDimension2D Transform3Dto2D(const Vector4D &vcPoint) = 0;





		//! Gets name of this video driver.
		/** \return Returns the name of the render driver, e.g. in case
		of the Direct3D11 driver, it would return "Direct3D 11". */
		virtual const String getVideoDriverName() const = 0;

		//! Get type of video driver
		/** \return Type of driver. */
		virtual SGP_DRIVER_TYPE getVideoDriverType() const = 0;

		//! Get the graphics card vendor name.
		virtual String getVendorName() = 0;

		//! Only used by the engine internally.
		/** \param color New color of the ambient light. */
		virtual void setAmbientLight(const Colour& color) = 0;

		//! Set SGP Engine working direction
		virtual void setWorkingDirection(const String& workingDir) = 0;


		//! Set & Get Render Device Time
		/** \param devicetime current virtual time in milliseconds
			\param deltatime virtual time elapsed since previous tick in milliseconds
		*/
		virtual void setRenderDeviceTime(uint32 devicetime, double deltatime) = 0;
		virtual uint32 getRenderDeviceTime() = 0;
		virtual double getDeltaTime() = 0;


		// EDITOR STUFF:
		//! Actually render the Editor rendered lines to color buffer.
		/** \param	bNoDepthLine	render lines not using scene depth, so all lines will on top of final scene
			\param  LineWidth		Specifies the width of rasterized lines.
        */    
		virtual void FlushEditorLinesRenderBatch(bool bNoDepthLine = false, float LineWidth = 1.0f ) = 0;


		// FONT STUFF:
		// Create one TTFFont
		virtual SGP_TTFFont* CreateTTFFont(FT_Library& FTLib) = 0;
		// Prepare something for Rendering text in screen. (must called before DrawTextInPos())
		virtual void BeginRenderText() = 0;
		// After something for Rendering text in screen. (must called after DrawTextInPos())
		virtual void EndRenderText() = 0;

		// Set current used font of SGP Engine for displaying in screen
		virtual void SetActiveFont(const char* FontName) = 0;

		// type like "Arial", Bold?, italic?, size, return succeed?
		virtual bool CreateFontInManager(const char* FontName, const String& FontFilePath, bool bBold, bool bItalic, uint16 iSize) = 0;

		// draw text: in postion x,y using active font, with color r, g, b, format string, variables
		virtual void DrawTextInPos(int xPos, int yPos, uint32 flag, float RealFontSize, uint8 r, uint8 g, uint8 b, wchar_t* format, ...) = 0;

		// draw text: in postion x,y as Left border, xmax as Right border, using active font, with color r, g, b, format string, variables
		// in the same time, also auto-wrap text, when Text out of range
		virtual void DrawTextAlignWrap(int xPos, int yPos, uint32 flag, float RealFontSize, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align, uint8 r, uint8 g, uint8 b, wchar_t* format, ...) = 0;
		
		// Precache Font texture (you can put most usually texts before Rendering starting)
		virtual void PreCacheChar(const String& sText) = 0;

	};


#endif		// __SGP_RENDERDEVICE_HEADER__