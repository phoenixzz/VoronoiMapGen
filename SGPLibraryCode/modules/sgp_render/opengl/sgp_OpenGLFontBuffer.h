#ifndef __SGP_OPENGLFONTBUFFER_HEADER__
#define __SGP_OPENGLFONTBUFFER_HEADER__

class COpenGLFontBuffer
{
public:
	COpenGLFontBuffer(uint32 TextureID, COpenGLRenderDevice* pDevice);
	~COpenGLFontBuffer();

	void	PushVertex(SGPVertex_FONT (&FourVerts)[4]);
	bool	IsBufferFull() { return( m_nNumTris >= MAX_CHAR_IN_BUFFER * 2 ); }
	void	Flush();



private:
	uint16							m_nNumTris;

	uint32							m_TextureID;
	COpenGLVertexBufferObject*		m_pVBO;

	COpenGLRenderDevice*			m_pRenderDevice;

private:
	static const unsigned short		MAX_CHAR_IN_BUFFER = 128;
	SGPVertex_FONT					*m_pCachedVertex;
	uint16							*m_pCachedIndex;
};


#endif		// __SGP_OPENGLFONTBUFFER_HEADER__