#ifndef __SGP_OPENGLPIXELBUFFEROBJECT_HEADER__
#define __SGP_OPENGLPIXELBUFFEROBJECT_HEADER__

class COpenGLPixelBufferObject
{
public:
	COpenGLPixelBufferObject(COpenGLRenderDevice* pRenderDevice) 
		: m_pRenderDevice(pRenderDevice), m_PBOID(0), m_target(GL_PIXEL_PACK_BUFFER) {}
	~COpenGLPixelBufferObject() 
	{
	}

	void createPBO();
	void bindPBO(GLenum target);
	void unbindPBO();
	void initPBOBuffer(uint32 iBufferSize, GLenum iUsageHint);
	void deletePBO();

	void* mapBufferToMemory(GLenum iUsageHint);
	void* mapSubBufferToMemory(GLenum iUsageHint, uint32 uiOffset, uint32 uiLength);
	void  unmapBuffer();


private:
	GLuint					m_PBOID;
	GLenum					m_target;

	COpenGLRenderDevice*	m_pRenderDevice;
};



#endif		// __SGP_OPENGLPIXELBUFFEROBJECT_HEADER__