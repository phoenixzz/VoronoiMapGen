
static uint16 FONT_index[6] = { 0, 1, 2, 2, 1, 3 };

COpenGLES2FontBuffer::COpenGLES2FontBuffer(uint32 OpenGLTexture, COpenGLES2RenderDevice* pDevice)
	: m_TextureID(OpenGLTexture), m_pRenderDevice(pDevice),
	  m_nNumTris(0)
{
	m_pVBO = new COpenGLES2VertexBufferObject( m_pRenderDevice );
	
	m_pVBO->createVAO();
	m_pVBO->bindVAO();

	m_pVBO->createVBO(SGPBT_VERTEX);
	m_pVBO->createVBO(SGPBT_INDEX);

	m_pVBO->bindVBO(SGPBT_VERTEX);
	m_pVBO->initVBOBuffer(SGPBT_VERTEX, MAX_CHAR_IN_BUFFER*4*sizeof(SGPVertex_FONT), GL_STREAM_DRAW);

	m_pVBO->setVAOPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(0));
	m_pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
	m_pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(8*sizeof(float)));

	m_pVBO->bindVBO(SGPBT_INDEX);
	m_pVBO->initVBOBuffer(SGPBT_INDEX, MAX_CHAR_IN_BUFFER*6*sizeof(uint16), GL_STREAM_DRAW);
	
	m_pVBO->unBindVAO();

	m_pCachedVertex = new SGPVertex_FONT [MAX_CHAR_IN_BUFFER*4];
	m_pCachedIndex = new uint16 [MAX_CHAR_IN_BUFFER*6];
}

COpenGLES2FontBuffer::~COpenGLES2FontBuffer()
{
	if(m_pVBO) 
	{
		m_pVBO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		m_pVBO->deleteVAO();

		delete m_pVBO;
		m_pVBO = NULL;
	}

	delete [] m_pCachedVertex;
	delete [] m_pCachedIndex;
	m_pCachedVertex = NULL;
	m_pCachedIndex = NULL;
}



void COpenGLES2FontBuffer::PushVertex(SGPVertex_FONT (&FourVerts)[4])
{
	float fWidth = (float)m_pRenderDevice->getViewPort().Width;
	float fHeight = (float)m_pRenderDevice->getViewPort().Height;

	// if the viewport in landscape mode, change font's aspect
	if( m_pRenderDevice->isViewportRotated() )
	{
		//fHeight = (float)m_pRenderDevice->getViewPort().Width;
		fWidth = (float)m_pRenderDevice->getViewPort().Height * (float)m_pRenderDevice->getViewPort().Height / (float)m_pRenderDevice->getViewPort().Width;
	}

	// Because OPENGL upside-down coordinates, we fliped Y position when Pushing Vertex
	// Y axis value is between (-1,1), fliped value is also (-1,1).
	FourVerts[0].x = FourVerts[0].x / fWidth * 2.0f - 1.0f;
	FourVerts[0].y = -FourVerts[0].y / fHeight * 2.0f + 1.0f;
	FourVerts[1].x = FourVerts[1].x / fWidth * 2.0f - 1.0f;
	FourVerts[1].y = -FourVerts[1].y / fHeight * 2.0f + 1.0f;
	FourVerts[2].x = FourVerts[2].x / fWidth * 2.0f - 1.0f;
	FourVerts[2].y = -FourVerts[2].y / fHeight * 2.0f + 1.0f;
	FourVerts[3].x = FourVerts[3].x / fWidth * 2.0f - 1.0f;
	FourVerts[3].y = -FourVerts[3].y / fHeight * 2.0f + 1.0f;

	m_pCachedVertex[m_nNumTris*2 + 0] = FourVerts[0];
	m_pCachedVertex[m_nNumTris*2 + 1] = FourVerts[1];
	m_pCachedVertex[m_nNumTris*2 + 2] = FourVerts[2];
	m_pCachedVertex[m_nNumTris*2 + 3] = FourVerts[3];

	// if the viewport in landscape mode, change font's xy position in screen
	if( m_pRenderDevice->isViewportRotated() )
	{
		m_pCachedVertex[m_nNumTris*2 + 0].x = -FourVerts[0].y;
		m_pCachedVertex[m_nNumTris*2 + 0].y = FourVerts[0].x;
		m_pCachedVertex[m_nNumTris*2 + 1].x = -FourVerts[1].y;
		m_pCachedVertex[m_nNumTris*2 + 1].y = FourVerts[1].x;
		m_pCachedVertex[m_nNumTris*2 + 2].x = -FourVerts[2].y;
		m_pCachedVertex[m_nNumTris*2 + 2].y = FourVerts[2].x;
		m_pCachedVertex[m_nNumTris*2 + 3].x = -FourVerts[3].y;
		m_pCachedVertex[m_nNumTris*2 + 3].y = FourVerts[3].x;
	}

	for( uint32 i=0; i<6; i++ )
		m_pCachedIndex[m_nNumTris*3+i] = uint16(FONT_index[i] + m_nNumTris*2);

	m_nNumTris += 2;
}

void COpenGLES2FontBuffer::Flush()
{
	if( m_nNumTris == 0 )
		return;


	m_pVBO->bindVBO(SGPBT_VERTEX);
	m_pVBO->initVBOBuffer(SGPBT_VERTEX, m_pCachedVertex, sizeof(SGPVertex_FONT)*m_nNumTris*2, GL_STREAM_DRAW);
	m_pVBO->bindVBO(SGPBT_INDEX);
	m_pVBO->initVBOBuffer(SGPBT_INDEX, m_pCachedIndex, sizeof(uint16)*m_nNumTris*3, GL_STREAM_DRAW);

	// Render
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_pRenderDevice->GetShaderManager());
	pShaderManager->GetGLSLShaderProgram(SGPST_UI)->useProgram();
	m_pRenderDevice->GetTextureManager()->getTextureByID(m_TextureID)->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(SGPST_UI)->setShaderUniform("gSampler0", 0);

	m_pVBO->bindVAO();
	glDrawElements( m_pRenderDevice->primitiveTypeToGL( SGPPT_TRIANGLES ),
					m_nNumTris*3, 
					GL_UNSIGNED_SHORT,
					(void*)0 );
	m_pVBO->unBindVAO();


	// Clear this Buffer
	m_nNumTris = 0;

	return;
}