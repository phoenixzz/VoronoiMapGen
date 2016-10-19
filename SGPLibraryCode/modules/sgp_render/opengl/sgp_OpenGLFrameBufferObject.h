#ifndef __SGP_OPENGLFRAMEBUFFEROBJECT_HEADER__
#define __SGP_OPENGLFRAMEBUFFEROBJECT_HEADER__


class COpenGLFrameBufferObject
{
public:
	COpenGLFrameBufferObject(COpenGLRenderDevice* pRenderDevice) 
		: m_pRenderDevice(pRenderDevice), m_FBOID(0), m_RBOID(0), 
		  m_FBOOpenGLDepthTextureID(0), m_FBOAuxiliaryTextureID(0), m_FBOColorAttachmentNum(0),
		  m_Width(0), m_Height(0)
	{
		m_FBOColorAttachments.insertMultiple(0, 0, COpenGLConfig::getInstance()->MaxColorAttachementPoints);
	}

	~COpenGLFrameBufferObject() 
	{
		deleteFBO();
	}


	// Creates a frame buffer object
	// Params:	FrameBufferName - framebuffer name (used for framebuffer texture name)
	//			nColorAttachmentNum - create how many Color Attachment for this FBO (Format is always RGBA8)
	//			width - framebuffer width
	//			height - framebuffer height
	//			bUseDepth - use depth renderbuffer, so rendering can perform depth testing
	//			bUseStencil - use depth + stencil renderbuffer, so rendering can perform stencil testing
	//			
	void createFBO( const String& FrameBufferName, uint32 nColorAttachmentNum, uint32 width, uint32 height, bool bUseDepth, bool bUseStencil );
	
	// Creates a AuxiliaryTexture for frame buffer object (Format is always RGBA8)
	// Params:	AuxiliaryTextureName - texture name (used for Auxiliary texture name)
	//			width - texture width
	//			height - texture height
	void createAuxiliaryTextureToFBO( const String& AuxiliaryTextureName, uint32 width, uint32 height );

	// Create a frame buffer object with FBO Depth Textures
	void createDepthTextureFBO(const String& FrameBufferName, uint32 width, uint32 height, bool bUseStencil);
	
	// Delete frame buffer object and free memory
	void deleteFBO();
	
	// Binds this frame buffer object
	// Params:	bSetFullViewport - set full framebuffer	viewport, default is true
	void bindFBO(bool bSetFullViewport = true);


	// switch for color attachment between textures in this FBO, textures should have same format and dimensions
	void switchFramebufferTexture(uint32 iAttachment, uint32 textureID);

	// Copy a block of pixels from the framebuffer ColorAttachment 0 to the default back buffer
	// Params:	srcX0, srcY0, srcX1, srcY1
    //          Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
    // Params:  dstX0, dstY0, dstX1, dstY1
    //          Specify the bounds of the destination rectangle within the write buffer of the write framebuffer.
	void blitFramebufferToBackBuffer(int32 srcX0, int32 srcY0, int32 srcX1, int32  srcY1, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1);
	
	// Copy a block of pixels from the framebuffer ColorAttachment srcRead to framebuffer ColorAttachment dstWrite
	void blitColorInFramebuffer(uint32 srcRead, uint32 dstWrite, int32 srcX0, int32 srcY0, int32 srcX1, int32  srcY1, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1);

	// Binds frame buffer texture
	// Params:	iTextureUnit - texture unit to bind texture to
	//			iColorAttachment - which color attachment texture to bind
	//			bDepthTexture - this frame buffer object is FBO Depth Textures?
	void bindFramebufferTexture(int iTextureUnit, int iColorAttachment, bool bDepthTexture = false);
	
	// UnBind this frame buffer object and back to normal window-system-provided framebuffer
	void unbindFBO();

	// Specifies a list of color buffers to be drawn into
	// Params:	count - the number of color buffers
	// For example, when count==2 color attachments (GL_COLOR_ATTACHMENT0..GL_COLOR_ATTACHMENT1)
	// when count==1 color attachments (GL_COLOR_ATTACHMENT0)
	void setDrawBuffersCount(int count);

	// Clear one color attachments color for this FBO
	// Also Specifies only one of color buffers to be drawn into (0->COLOR_ATTACHMENT0,1->COLOR_ATTACHMENT1)
	// Params:	nColorAttachment - which color buffers to be clear and drawn into
	//			fRed, fGreen, fBlue, fAlpha - set clear color of color buffer
	//			when nColorAttachment==0 color attachments (GL_COLOR_ATTACHMENT0)
	//			when nColorAttachment==1 color attachments (GL_COLOR_ATTACHMENT1)
	//			bClearDepth - true for also clear depth
	void clearOneDrawBuffers(int nColorAttachment, float fRed, float fGreen, float fBlue, float fAlpha, bool bClearDepth );





	inline uint32 getWidth() { return m_Width; }
	inline uint32 getHeight() { return m_Height; }
	
	inline uint32 getAuxiliaryTextureID() { return m_FBOAuxiliaryTextureID; }
	inline uint32 getColorAttachmentsTextureID(int iAttachment) { return m_FBOColorAttachments[iAttachment]; }
	inline uint32 getColorAttachmentNumber() { return m_FBOColorAttachmentNum; }

private:
	bool checkFramebufferStatus();
	
private:
	GLuint					m_FBOID;					// ID of Frame Buffer Object
	GLuint					m_RBOID;					// ID of Renderbuffer object for depth & stencil
	GLuint					m_FBOOpenGLDepthTextureID;	// Opengl resource id for FBO Depth Texture
	
	Array<uint32>			m_FBOColorAttachments;		// SGP Texture ID for FBO color attachment point( color attachment 0 -- color attachment x )
	uint32					m_FBOColorAttachmentNum;	// how many Color Attachment for this FBO
	
	uint32					m_FBOAuxiliaryTextureID;	// Auxiliary mapping for FBO (used for water reflection mapping) (not attach to FBO) 


	uint32					m_Width;					// framebuffer width
	uint32					m_Height;					// framebuffer height

	COpenGLRenderDevice*	m_pRenderDevice;
};

#endif		// __SGP_OPENGLFRAMEBUFFEROBJECT_HEADER__