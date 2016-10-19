
/*-----------------------------------------------
Name:	createPBO
Params:	null
		
Result:	Creates Pixel buffer object.
/*---------------------------------------------*/
void COpenGLPixelBufferObject::createPBO()
{
	m_pRenderDevice->extGlGenBuffers(1, &m_PBOID);
}

/*-----------------------------------------------
Name:	deletePBO
Params:	null
Result:	Releases PBO and frees all memory.
/*---------------------------------------------*/
void COpenGLPixelBufferObject::deletePBO()
{
	if( m_PBOID )
		m_pRenderDevice->extGlDeleteBuffers(1, &m_PBOID);
}

/*-----------------------------------------------
Name:	bindPBO / unbindPBO
Params:	
Result:	Binds / Unbind this PBO.
/*---------------------------------------------*/
void COpenGLPixelBufferObject::bindPBO(GLenum target)
{
	m_target = target;
	m_pRenderDevice->extGlBindBuffer(target, m_PBOID);	  
}

void COpenGLPixelBufferObject::unbindPBO()
{
	m_pRenderDevice->extGlBindBuffer(m_target, NULL);	
}



/*-----------------------------------------------
Name:	initPBOBuffer
Params:	iBufferSize - the size in bytes of the buffer object's new data store.
		iUsageHint - GL_STREAM_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_READ,  GL_DYNAMIC_READ...
Result:	Init one Pixel Buffer Object.
/*---------------------------------------------*/
void COpenGLPixelBufferObject::initPBOBuffer(uint32 iBufferSize, GLenum iUsageHint)
{
	m_pRenderDevice->extGlBufferData(m_target, iBufferSize, NULL, iUsageHint);
}

/*-----------------------------------------------
Name:	mapBufferToMemory
Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
Result:	Maps whole buffer data to memory and returns pointer to data.
/*---------------------------------------------*/
void* COpenGLPixelBufferObject::mapBufferToMemory(GLenum iUsageHint)
{
	void* ptrRes = NULL;
	ptrRes = m_pRenderDevice->extGlMapBuffer(m_target, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------
Name:	mapSubBufferToMemory
Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
		uiOffset - data offset (from where should data be mapped).
		uiLength - length of data
Result:	Maps specified part of buffer to memory.
/*---------------------------------------------*/
void* COpenGLPixelBufferObject::mapSubBufferToMemory(GLenum iUsageHint, uint32 uiOffset, uint32 uiLength)
{
	void* ptrRes = NULL;
	ptrRes = m_pRenderDevice->extGlMapBufferRange(m_target, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------
Name:	unmapBuffer
Params:	
Result:	Unmaps previously mapped buffer.
/*---------------------------------------------*/
void COpenGLPixelBufferObject::unmapBuffer()
{
	m_pRenderDevice->extGlUnmapBuffer(m_target);
}