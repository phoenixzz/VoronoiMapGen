#ifndef __SGP_OPENGLES2FRAMEBUFFEROBJECT_HEADER__
#define __SGP_OPENGLES2FRAMEBUFFEROBJECT_HEADER__


class COpenGLES2FrameBufferObject
{
public:
	COpenGLES2FrameBufferObject(COpenGLES2RenderDevice* pRenderDevice) 
		:	m_pRenderDevice(pRenderDevice),
			m_FBOID(0), m_RBOID(0), 
			m_FBOOpenGLDepthTextureID(0), 
			m_FBORenderToTextureID(0),
			m_Width(0), m_Height(0)			
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_MainBackBufferFBO);		
	}

	~COpenGLES2FrameBufferObject() 
	{
		deleteFBO();
	}


	// Creates a frame buffer object
	// Params:	FrameBufferName - framebuffer name (used for framebuffer texture name)
	//			width - framebuffer width
	//			height - framebuffer height
	//			bUseDepth - use depth renderbuffer, so rendering can perform depth testing
	//			bUseStencil - use depth + stencil renderbuffer, so rendering can perform stencil testing
	//			
	void createFBO( const String& FrameBufferName, uint32 width, uint32 height, bool bUseDepth, bool bUseStencil );
	
	// Create a frame buffer object with FBO Depth Textures
	void createDepthTextureFBO(const String& FrameBufferName, uint32 width, uint32 height, bool bUseStencil);
	
	// Delete frame buffer object and free memory
	void deleteFBO();
	
	// Binds this frame buffer object
	// Params:	bSetFullViewport - set full framebuffer	viewport, default is true
	void bindFBO(bool bSetFullViewport = true);


	// Binds frame buffer texture
	// Params:	iTextureUnit - texture unit to bind texture to
	//			bDepthTexture - this frame buffer object is FBO Depth Textures?
	void bindFramebufferTexture(int iTextureUnit, bool bDepthTexture = false);
	
	// UnBind this frame buffer object and back to normal window-system-provided framebuffer
	void unbindFBO();


	inline uint32 getWidth() { return m_Width; }
	inline uint32 getHeight() { return m_Height; }
	
	inline uint32 getFBORenderToTextureID() { return m_FBORenderToTextureID; }


private:
	bool checkFramebufferStatus();
	
private:
	// OpenGL Resource
	GLuint					m_FBOID;					// ID of Frame Buffer Object
	GLuint					m_RBOID;					// ID of Renderbuffer object for depth & stencil

	GLuint					m_FBOOpenGLDepthTextureID;	// Opengl resource id for FBO Depth Texture
	GLint					m_MainBackBufferFBO;		// currently bound frame buffer object. On most platforms this just gives 0

	// SGP Resource
	uint32					m_FBORenderToTextureID;		// SGP Texture ID for FBO color attachment point( color attachment 0 )

	uint32					m_Width;					// framebuffer width
	uint32					m_Height;					// framebuffer height

	COpenGLES2RenderDevice*	m_pRenderDevice;
};

#endif		// __SGP_OPENGLES2FRAMEBUFFEROBJECT_HEADER__