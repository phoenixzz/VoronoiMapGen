

void COpenGLES2FrameBufferObject::createFBO(const String& FrameBufferName, uint32 width, uint32 height, bool bUseDepth, bool bUseStencil)
{
	m_Width = width;
	m_Height = height;

	// set up color texture
	SDimension2D FrameBufferSize(width, height);

	uint32 FBOTextureID = m_pRenderDevice->GetTextureManager()->registerEmptyTexture(FrameBufferSize, String("FrameBuffer ") + FrameBufferName + String("Color0 "), SGPPF_A8R8G8B8);
	COpenGLES2Texture *pOpenGLTexture = static_cast<COpenGLES2Texture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(FBOTextureID)->pSGPTexture );
	pOpenGLTexture->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
	pOpenGLTexture->setWrapMode(TEXTURE_ADDRESS_CLAMP_TO_EDGE, TEXTURE_ADDRESS_CLAMP_TO_EDGE);
	pOpenGLTexture->unBindTexture2D(0);
	m_FBORenderToTextureID = FBOTextureID;


	if( bUseDepth || bUseStencil )
	{
		// create a render buffer object for the depth buffer
		glGenRenderbuffers(1, &m_RBOID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RBOID);
		
		// create the render buffer in the GPU
		if( bUseStencil && bUseDepth )
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, m_Width, m_Height);
		else if( bUseStencil )
			glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, m_Width, m_Height);
		else if( bUseDepth )
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_Width, m_Height);
		
		// unbind the render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	// create a framebuffer object
	glGenFramebuffers(1, &m_FBOID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

    // attach a texture to FBO color attachement point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pOpenGLTexture->getOpenGLTextureID(), 0);

    // attach a renderbuffer to stencil attachment point
	// AND attach a renderbuffer to depth attachment point
	if( bUseStencil )
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBOID);
	if( bUseDepth )
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBOID);


    //@@ disable color buffer if you don't attach any color buffer image,
    //@@ for example, rendering depth buffer only to a texture.
    //@@ Otherwise, extGlCheckFramebufferStatus will not be complete.
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

	// check the frame buffer
	if( !checkFramebufferStatus() )
		Logger::getCurrentLogger()->writeToLog(String("Create Frame Buffer Object Error!"), ELL_ERROR);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_MainBackBufferFBO);
}


void COpenGLES2FrameBufferObject::createDepthTextureFBO(const String& , uint32 width, uint32 height, bool bUseStencil)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_OES, m_Width, m_Height, 0, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	// create a render buffer object for the depth buffer
	glGenRenderbuffers(1, &m_RBOID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBOID);
	
	// create the render buffer in the GPU
	if( bUseStencil )
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, m_Width, m_Height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);

	// unbind the render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// create a framebuffer object
	glGenFramebuffers(1, &m_FBOID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

	// attach a texture to FBO depth or stencil attachement point
	if( bUseStencil )
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID, 0);

    //@@ disable color buffer if you don't attach any color buffer image,
    //@@ for example, rendering depth buffer only to a texture.
    //@@ Otherwise, extGlCheckFramebufferStatus will not be complete.
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

	// check the frame buffer
	if( !checkFramebufferStatus() )
		Logger::getCurrentLogger()->writeToLog(String("Create Frame Buffer Object Error!"), ELL_ERROR);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_MainBackBufferFBO);
}


// Deletes frame buffer object and free memory.
void COpenGLES2FrameBufferObject::deleteFBO()
{
	if( m_FBORenderToTextureID != 0 )
	{
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_FBORenderToTextureID);
		m_FBORenderToTextureID = 0;
	}


	if(m_FBOID)
	{
		glDeleteFramebuffers(1, &m_FBOID);
		m_FBOID = 0;
	}
	if(m_RBOID)
	{
		glDeleteRenderbuffers(1, &m_RBOID);
		m_RBOID = 0;
	}

	if( m_FBOOpenGLDepthTextureID )
	{
		glDeleteTextures(1, &m_FBOOpenGLDepthTextureID);
		m_FBOOpenGLDepthTextureID = 0;
	}
}

void COpenGLES2FrameBufferObject::bindFBO(bool bSetFullViewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);

	if( bSetFullViewport )
		glViewport(0, 0, m_Width, m_Height);
}



void COpenGLES2FrameBufferObject::bindFramebufferTexture(int iTextureUnit, bool bDepthTexture)
{
	if( bDepthTexture )
	{
		glActiveTexture(GL_TEXTURE0 + iTextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_FBOOpenGLDepthTextureID);
	}
	else
	{
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_FBORenderToTextureID)->pSGPTexture->BindTexture2D(iTextureUnit);
	}
}

void COpenGLES2FrameBufferObject::unbindFBO()
{
	// back to normal window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_MainBackBufferFBO);	
}





bool COpenGLES2FrameBufferObject::checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Attachment is NOT complete."), ELL_ERROR);
		return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Not all attached images have the same width and height."), ELL_ERROR);
        return false;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: No images are attached to the framebuffer."), ELL_ERROR);
		return false;
 
    case GL_FRAMEBUFFER_UNSUPPORTED:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Unsupported by FBO implementation."), ELL_ERROR);
        return false;

    default:
		Logger::getCurrentLogger()->writeToLog(String("Framebuffer incomplete: Unknown error."), ELL_ERROR);
        return false;
    }
}