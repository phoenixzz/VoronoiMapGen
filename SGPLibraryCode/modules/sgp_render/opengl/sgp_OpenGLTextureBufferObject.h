#ifndef __SGP_OPENGLTEXTUREBUFFEROBJECT_HEADER__
#define __SGP_OPENGLTEXTUREBUFFEROBJECT_HEADER__

class COpenGLTextureBufferObject
{
public:
	COpenGLTextureBufferObject(COpenGLRenderDevice* pRenderDevice) 
		: m_pRenderDevice(pRenderDevice), m_TBOID(0), m_TBOTextureID(0) {}
	~COpenGLTextureBufferObject() 
	{
	}

	void createTBO();
	void bindTBO();
	void unbindTBO();
	void initTBOBuffer(int nJointCount, GLenum iUsageHint);
	void deleteTBO();

	void* mapBufferToMemory(GLenum iUsageHint);
	void* mapSubBufferToMemory(GLenum iUsageHint, uint32 uiOffset, uint32 uiLength);
	void  unmapBuffer();

	bool bindTextureBuffer( int iTextureUnit );

private:
	GLuint					m_TBOID;
	GLuint					m_TBOTextureID;


	COpenGLRenderDevice*	m_pRenderDevice;
};



#endif		// __SGP_OPENGLTEXTUREBUFFEROBJECT_HEADER__