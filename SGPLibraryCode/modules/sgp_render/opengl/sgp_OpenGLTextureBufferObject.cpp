
/*-----------------------------------------------
Name:	createTBO
Params:	null
		
Result:	Creates texture buffer object.
/*---------------------------------------------*/
void COpenGLTextureBufferObject::createTBO()
{
	m_pRenderDevice->extGlGenBuffers(1, &m_TBOID);
	glGenTextures(1, &m_TBOTextureID);
}

/*-----------------------------------------------
Name:	deleteTBO
Params:	null
Result:	Releases TBO and frees all memory.
/*---------------------------------------------*/
void COpenGLTextureBufferObject::deleteTBO()
{
	if( m_TBOID )
		m_pRenderDevice->extGlDeleteBuffers(1, &m_TBOID);
	if( m_TBOTextureID )
		glDeleteTextures(1, &m_TBOTextureID);
}

/*-----------------------------------------------
Name:	BindTBO / unbindTBO
Params:	
Result:	Binds / Unbind this TBO.
/*---------------------------------------------*/
void COpenGLTextureBufferObject::bindTBO()
{
	m_pRenderDevice->extGlBindBuffer(GL_TEXTURE_BUFFER, m_TBOID);	  
}

void COpenGLTextureBufferObject::unbindTBO()
{
	m_pRenderDevice->extGlBindBuffer(GL_TEXTURE_BUFFER, NULL);	
}

/*-----------------------------------------------
/*Name:		! bind the current Texture
Params:		 
			iTextureUnit - texture unit to bind texture to
Result:		Returns whether setting was a success or not.
---------------------------------------------*/
bool COpenGLTextureBufferObject::bindTextureBuffer( int iTextureUnit )
{
	if(iTextureUnit >= m_pRenderDevice->MaxTextureUnits)
		return false;

	if(m_pRenderDevice->MultiTextureExtension)
		m_pRenderDevice->extGlActiveTexture(GL_TEXTURE0_ARB + iTextureUnit);

	glBindTexture(GL_TEXTURE_BUFFER, m_TBOTextureID);
	
	return true;
}

/*-----------------------------------------------
Name:	initTBOBuffer
Params:	nJointCount - bone joint count
		iUsageHint - GL_STREAM_DRAW, GL_DYNAMIC_DRAW...
Result:	Init one Texture Buffer Object.
/*---------------------------------------------*/
void COpenGLTextureBufferObject::initTBOBuffer(int nJointCount, GLenum iUsageHint)
{
	m_pRenderDevice->extGlBufferData(GL_TEXTURE_BUFFER, 12 * nJointCount * sizeof(GLfloat), NULL, iUsageHint);

	bindTextureBuffer(0);

	m_pRenderDevice->extGlTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_TBOID);

}

/*-----------------------------------------------
Name:	mapBufferToMemory
Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
Result:	Maps whole buffer data to memory and returns pointer to data.
/*---------------------------------------------*/
void* COpenGLTextureBufferObject::mapBufferToMemory(GLenum iUsageHint)
{
	void* ptrRes = NULL;
	ptrRes = m_pRenderDevice->extGlMapBuffer(GL_TEXTURE_BUFFER, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------
Name:	mapSubBufferToMemory
Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
		uiOffset - data offset (from where should data be mapped).
		uiLength - length of data
Result:	Maps specified part of buffer to memory.
/*---------------------------------------------*/
void* COpenGLTextureBufferObject::mapSubBufferToMemory(GLenum iUsageHint, uint32 uiOffset, uint32 uiLength)
{
	void* ptrRes = NULL;
	ptrRes = m_pRenderDevice->extGlMapBufferRange(GL_TEXTURE_BUFFER, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------
Name:	unmapBuffer
Params:	
Result:	Unmaps previously mapped buffer.
/*---------------------------------------------*/
void COpenGLTextureBufferObject::unmapBuffer()
{
	//m_pRenderDevice->extGlBindBuffer(GL_TEXTURE_BUFFER, m_TBOID);
	m_pRenderDevice->extGlUnmapBuffer(GL_TEXTURE_BUFFER);
}