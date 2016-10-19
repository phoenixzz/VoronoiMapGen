

void COpenGLFrameBufferObject::createFBO(const String& FrameBufferName, uint32 nColorAttachmentNum, uint32 width, uint32 height, bool bUseDepth, bool bUseStencil)
{
	m_Width = width;
	m_Height = height;

	m_FBOColorAttachmentNum = nColorAttachmentNum;

	// set up color texture
	SDimension2D FrameBufferSize(width, height);
	for( uint32 i=0; i<nColorAttachmentNum; i++ )
	{
		uint32 FBOTextureID = m_pRenderDevice->GetTextureManager()->registerEmptyTexture(FrameBufferSize, String("FrameBuffer ") + FrameBufferName + String("Color ") + String(i), SGPPF_A8R8G8B8);
		COpenGLTexture *pOpenGLTexture = static_cast<COpenGLTexture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(FBOTextureID)->pSGPTexture );
		pOpenGLTexture->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		pOpenGLTexture->setWrapMode(TEXTURE_ADDRESS_CLAMP_TO_EDGE, TEXTURE_ADDRESS_CLAMP_TO_EDGE);
		pOpenGLTexture->unBindTexture2D(0);
		m_FBOColorAttachments.set(i, FBOTextureID);
	}

	if( bUseDepth || bUseStencil )
	{
		// create a render buffer object for the depth buffer
		m_pRenderDevice->extGlGenRenderbuffers(1, &m_RBOID);
		m_pRenderDevice->extGlBindRenderbuffer(GL_RENDERBUFFER, m_RBOID);
		
		// create the render buffer in the GPU
		if( bUseStencil )
			m_pRenderDevice->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		else if( bUseDepth )
			m_pRenderDevice->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
		
		// unbind the render buffer
		m_pRenderDevice->extGlBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	// create a framebuffer object
	m_pRenderDevice->extGlGenFramebuffers(1, &m_FBOID);
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

    // attach a texture to FBO color attachement point
	for( uint32 i=0; i<nColorAttachmentNum; i++ )
	{
		COpenGLTexture *pOpenGLTexture = static_cast<COpenGLTexture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(m_FBOColorAttachments[i])->pSGPTexture );
		m_pRenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, pOpenGLTexture->getOpenGLTextureID(), 0);
	}
 
    // attach a renderbuffer to stencil attachment point
	// AND attach a renderbuffer to depth attachment point
	if( bUseStencil )
		m_pRenderDevice->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBOID);
	else if( bUseDepth )
		m_pRenderDevice->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBOID);


    //@@ disable color buffer if you don't attach any color buffer image,
    //@@ for example, rendering depth buffer only to a texture.
    //@@ Otherwise, extGlCheckFramebufferStatus will not be complete.
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

	// check the frame buffer
	if( !checkFramebufferStatus() )
		Logger::getCurrentLogger()->writeToLog(String("Create Frame Buffer Object Error!"), ELL_ERROR);
	
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void COpenGLFrameBufferObject::createDepthTextureFBO(const String& , uint32 width, uint32 height, bool bUseStencil)
{
	m_Width = width;
	m_Height = height;

    // create a depth texture object
    glGenTextures(1, &m_FBOOpenGLDepthTextureID);
    glBindTexture(GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID);

	// set depth texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create the depth texture in the GPU
	if( bUseStencil )
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, 0);


	// create a render buffer object for the depth buffer
	m_pRenderDevice->extGlGenRenderbuffers(1, &m_RBOID);
	m_pRenderDevice->extGlBindRenderbuffer(GL_RENDERBUFFER, m_RBOID);
	
	// create the render buffer in the GPU
	if( bUseStencil )
		m_pRenderDevice->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	else
		m_pRenderDevice->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);

	// unbind the render buffer
	m_pRenderDevice->extGlBindRenderbuffer(GL_RENDERBUFFER, 0);

	// create a framebuffer object
	m_pRenderDevice->extGlGenFramebuffers(1, &m_FBOID);
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

	// attach a texture to FBO depth or stencil attachement point
	if( bUseStencil )
		m_pRenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID, 0);
	else
		m_pRenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID, 0);

    //@@ disable color buffer if you don't attach any color buffer image,
    //@@ for example, rendering depth buffer only to a texture.
    //@@ Otherwise, extGlCheckFramebufferStatus will not be complete.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

	// check the frame buffer
	if( !checkFramebufferStatus() )
		Logger::getCurrentLogger()->writeToLog(String("Create Frame Buffer Object Error!"), ELL_ERROR);
	
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);

}


// Deletes frame buffer object and free memory.
void COpenGLFrameBufferObject::deleteFBO()
{
	for( int i=0; i<m_FBOColorAttachments.size(); i++ )
	{
		if( m_FBOColorAttachments[i] != 0 )
		{
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_FBOColorAttachments[i]);
			m_FBOColorAttachments.set(i, 0);
		}
	}

	if(m_FBOID)
	{
		m_pRenderDevice->extGlDeleteFramebuffers(1, &m_FBOID);
		m_FBOID = 0;
	}
	if(m_RBOID)
	{
		m_pRenderDevice->extGlDeleteRenderbuffers(1, &m_RBOID);
		m_RBOID = 0;
	}

	if( m_FBOOpenGLDepthTextureID )
	{
		glDeleteTextures(1, &m_FBOOpenGLDepthTextureID);
		m_FBOOpenGLDepthTextureID = 0;
	}

	if( m_FBOAuxiliaryTextureID != 0 )
	{
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_FBOAuxiliaryTextureID);
		m_FBOAuxiliaryTextureID = 0;
	}
}

void COpenGLFrameBufferObject::bindFBO(bool bSetFullViewport)
{
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

	if( bSetFullViewport )
		glViewport(0, 0, m_Width, m_Height);
}

void COpenGLFrameBufferObject::createAuxiliaryTextureToFBO( const String& AuxiliaryTextureName, uint32 width, uint32 height )
{
	SDimension2D FrameBufferSize(width, height);

	m_FBOAuxiliaryTextureID = m_pRenderDevice->GetTextureManager()->registerEmptyTexture(FrameBufferSize, String("FrameBuffer Auxiliary ") + AuxiliaryTextureName, SGPPF_A8R8G8B8);
	COpenGLTexture *pOpenGLTexture = static_cast<COpenGLTexture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(m_FBOAuxiliaryTextureID)->pSGPTexture );
	pOpenGLTexture->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
	pOpenGLTexture->setWrapMode(TEXTURE_ADDRESS_CLAMP_TO_EDGE, TEXTURE_ADDRESS_CLAMP_TO_EDGE);
	pOpenGLTexture->unBindTexture2D(0);
}

void COpenGLFrameBufferObject::blitFramebufferToBackBuffer(int32 srcX0, int32 srcY0, int32 srcX1, int32 srcY1, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1)
{
	// select the FBO color attachment 0 to copy from
    m_pRenderDevice->extGlBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOID);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	// bind the destination framebuffer to copy to
    m_pRenderDevice->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
     
    m_pRenderDevice->extGlBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, 
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Reset to default main buffer
    m_pRenderDevice->extGlBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    m_pRenderDevice->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void COpenGLFrameBufferObject::blitColorInFramebuffer(uint32 srcRead, uint32 dstWrite, int32 srcX0, int32 srcY0, int32 srcX1, int32  srcY1, int32 dstX0, int32 dstY0, int32 dstX1, int32 dstY1)
{
	// bind the source framebuffer and select a color attachment to copy from
	m_pRenderDevice->extGlBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOID);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + srcRead);
	// bind the destination framebuffer and select the color attachments to copy to
    m_pRenderDevice->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBOID);
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 + dstWrite };
	m_pRenderDevice->extGlDrawBuffers(1, attachments);

    m_pRenderDevice->extGlBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, 
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Reset to default 
    glReadBuffer(GL_COLOR_ATTACHMENT0);
	GLuint RestoreAttach[1] = { GL_COLOR_ATTACHMENT0 };
	m_pRenderDevice->extGlDrawBuffers(1, RestoreAttach);
}

void COpenGLFrameBufferObject::switchFramebufferTexture(uint32 iAttachment, uint32 textureID)
{
	COpenGLTexture *pOpenGLTexture = static_cast<COpenGLTexture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(textureID)->pSGPTexture );
	m_pRenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+iAttachment, GL_TEXTURE_2D, pOpenGLTexture->getOpenGLTextureID(), 0);
}

void COpenGLFrameBufferObject::bindFramebufferTexture(int iTextureUnit, int iColorAttachment, bool bDepthTexture)
{
	if( bDepthTexture )
	{
		if(m_pRenderDevice->MultiTextureExtension)
			m_pRenderDevice->extGlActiveTexture(GL_TEXTURE0_ARB + iTextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID);
	}
	else
	{
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_FBOColorAttachments[iColorAttachment])->pSGPTexture->BindTexture2D(iTextureUnit);
	}
}

void COpenGLFrameBufferObject::unbindFBO()
{
	// back to normal window-system-provided framebuffer
	m_pRenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);	
}


void COpenGLFrameBufferObject::setDrawBuffersCount(int count)
{
	static GLenum twobuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	static GLenum onebuffers[] = {GL_COLOR_ATTACHMENT0};

	if( count > 1 )
		m_pRenderDevice->extGlDrawBuffers(2, twobuffers);
	else
		m_pRenderDevice->extGlDrawBuffers(1, onebuffers);
}

void COpenGLFrameBufferObject::clearOneDrawBuffers(int nColorAttachment, float fRed, float fGreen, float fBlue, float fAlpha, bool bClearDepth )
{
	static GLenum Firstbuffers[] = {GL_COLOR_ATTACHMENT0};
	static GLenum Secondbuffers[] = {GL_COLOR_ATTACHMENT1};

	switch( nColorAttachment )
	{
	case 0:
		m_pRenderDevice->extGlDrawBuffers(1, Firstbuffers);		// GL_COLOR_ATTACHMENT0
		glClearColor(fRed, fGreen, fBlue, fAlpha);
		glClear( bClearDepth ? GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT : GL_COLOR_BUFFER_BIT );
		break;
	case 1:
		m_pRenderDevice->extGlDrawBuffers(1, Secondbuffers);	// GL_COLOR_ATTACHMENT1
		glClearColor(fRed, fGreen, fBlue, fAlpha);
		glClear( bClearDepth ? GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT : GL_COLOR_BUFFER_BIT );
		break;
	}
}



bool COpenGLFrameBufferObject::checkFramebufferStatus()
{
    // check FBO status
    GLenum status = m_pRenderDevice->extGlCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Attachment is NOT complete."), ELL_ERROR);
		return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: No image is attached to FBO."), ELL_ERROR);
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Draw buffer."), ELL_ERROR);
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Read buffer."), ELL_ERROR);
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Unsupported by FBO implementation."), ELL_ERROR);
        return false;

    default:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Unknown error."), ELL_ERROR);
        return false;
    }
}