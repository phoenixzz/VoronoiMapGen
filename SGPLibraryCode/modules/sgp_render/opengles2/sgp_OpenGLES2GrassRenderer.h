#ifndef __SGP_OPENGLES2GRASSRENDER_HEADER__
#define __SGP_OPENGLES2GRASSRENDER_HEADER__

class COpenGLES2GrassRenderer
{
public:
	COpenGLES2GrassRenderer(COpenGLES2RenderDevice *pRenderDevice);
	~COpenGLES2GrassRenderer();

	void initializeFromGrass(CSGPGrass* pGrass);

	void releaseGrassTexture();

	void render(CSGPGrass* pGrass);

	void update(float fDeltaTimeInSecond, const Vector4D& camPos, const Frustum& viewFrustum, CSGPGrass* pGrass);

	void DoDrawGrass();


private:
	void createGrassTexture(const String& GrassTextureName);
	

private:
	static const int INIT_GRASSCLUSTERINSTANCE_NUM = 2048;
	struct Sorter
    {
		Sorter(COpenGLES2RenderDevice* pRenderDevice) { m_pRD = pRenderDevice; }
		static COpenGLES2RenderDevice* m_pRD;
		static int compareElements( SGPVertex_GRASS_Cluster first, SGPVertex_GRASS_Cluster second ) noexcept;
	};
	Array<SGPVertex_GRASS_Cluster> m_GrassClusterInstanceArray;		// GrassCluster data array


	SGPVertex_GRASS				m_grassVertex[4*3];


	COpenGLES2RenderDevice*		m_pRenderDevice;
	uint32						m_GrassTextureID;			// Grass texture ID
	Vector4D					m_vCameraPos;				// current camera position

	GLuint						m_nGrassClusterVBOID;		// Grass vertex VBO
	GLuint						m_nGrassClusterIndexVBOID;	// Grass index VBO
	GLuint						m_nGrassClusterVAOID;		// Grass vertex VAO

	Vector4D					m_vWindDirForce;			// wind params for rendering (Wind Direction And Strength)
	Vector2D					m_vTimeParams;				// time params for rendering (x is time ; y is grass movement period)
	Vector4D					m_vTextureAtlas;			// tiled texture UV params for rendering
	
	Vector3D					m_vDefaultGrassSize;		// Default grass Quad size
};

#endif		// __SGP_OPENGLES2GRASSRENDER_HEADER__