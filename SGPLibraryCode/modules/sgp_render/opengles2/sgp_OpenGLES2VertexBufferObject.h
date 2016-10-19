#ifndef __SGP_OPENGLES2VERTEXBUFFEROBJECT_HEADER__
#define __SGP_OPENGLES2VERTEXBUFFEROBJECT_HEADER__

class COpenGLES2VertexBufferObject
{
public:
	COpenGLES2VertexBufferObject(COpenGLES2RenderDevice* pRenderDevice) 
		: m_bDataUploaded(true), m_bIndexUploaded(true), m_pRenderDevice(pRenderDevice) {}
	~COpenGLES2VertexBufferObject() {}

	void createVAO();
	void bindVAO();
	void unBindVAO();
	void deleteVAO();
	void setVAOPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

	void createVBO(SGP_BUFFER_TYPE BufferType);	
	void deleteVBO(SGP_BUFFER_TYPE BufferType);


	void* mapBufferToMemory(SGP_BUFFER_TYPE BufferType, GLenum iUsageHint);
	void* mapSubBufferToMemory(SGP_BUFFER_TYPE BufferType, GLenum iUsageHint, uint32 uiOffset, uint32 uiLength);
	void  unmapBuffer(SGP_BUFFER_TYPE BufferType);


	void bindVBO(SGP_BUFFER_TYPE BufferType);
	void initVBOBuffer(SGP_BUFFER_TYPE BufferType, int iBufferSize, GLenum iUsageHint);
	void initVBOBuffer(SGP_BUFFER_TYPE BufferType, const void* ptrData, int iBufferSize, GLenum iUsageHint);
	void uploadDataToGPU(SGP_BUFFER_TYPE BufferType, GLenum iUsageHint);
	
	void ensureDataSpace(SGP_BUFFER_TYPE buffertype, uint32 uiDataSize);	
	void addData(SGP_BUFFER_TYPE buffertype, const void* ptrData, uint32 uiDataSize);


	void* getDataPointer(SGP_BUFFER_TYPE buffertype);
	GLuint getVBOBufferID(SGP_BUFFER_TYPE buffertype)
	{ 
		if( buffertype == SGPBT_VERTEX ) 
			return m_VBOBufferID[0];
		else if( buffertype == SGPBT_INDEX ) 
			return m_VBOBufferID[1];
		else
			return 0;
	}

private:
	GLuint					m_VAOID;

	GLuint					m_VBOBufferID[2];


	bool					m_bDataUploaded;
	bool					m_bIndexUploaded;

	Array<uint8>			m_Data;
	Array<uint16>			m_Index;

	COpenGLES2RenderDevice*	m_pRenderDevice;
};

#endif		// __SGP_OPENGLES2VERTEXBUFFEROBJECT_HEADER__