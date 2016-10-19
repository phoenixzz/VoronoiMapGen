#ifndef __SGP_OPENGLES2FONTBUFFER_HEADER__
#define __SGP_OPENGLES2FONTBUFFER_HEADER__

class COpenGLES2FontBuffer
{
public:
	COpenGLES2FontBuffer(uint32 TextureID, COpenGLES2RenderDevice* pDevice);
	~COpenGLES2FontBuffer();

	void	PushVertex(SGPVertex_FONT (&FourVerts)[4]);
	bool	IsBufferFull() { return( m_nNumTris >= MAX_CHAR_IN_BUFFER * 2 ); }
	void	Flush();



private:
	uint16							m_nNumTris;

	uint32							m_TextureID;
	COpenGLES2VertexBufferObject*	m_pVBO;

	COpenGLES2RenderDevice*			m_pRenderDevice;

private:
	static const unsigned short		MAX_CHAR_IN_BUFFER = 128;
	SGPVertex_FONT					*m_pCachedVertex;
	uint16							*m_pCachedIndex;
};


#endif		// __SGP_OPENGLES2FONTBUFFER_HEADER__