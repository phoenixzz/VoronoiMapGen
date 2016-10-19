#ifndef __SGP_OPENGLGRASSRENDER_HEADER__
#define __SGP_OPENGLGRASSRENDER_HEADER__

class COpenGLGrassRenderer
{
public:
	COpenGLGrassRenderer(COpenGLRenderDevice *pRenderDevice);
	~COpenGLGrassRenderer();

	void initializeFromGrass(CSGPGrass* pGrass);

	void releaseGrassTexture();

	void render(CSGPGrass* pGrass);

	void update(float fDeltaTimeInSecond, const Vector4D& camPos, const Frustum& viewFrustum, CSGPGrass* pGrass);

	void DoDrawGrassInstance();


private:
	void createGrassTexture(const String& GrassTextureName);
	

private:
	static const int INIT_GRASSCLUSTERINSTANCE_NUM = 8192;
	struct Sorter
    {
		Sorter(COpenGLRenderDevice*	pRenderDevice) { m_pRD = pRenderDevice; }
		static COpenGLRenderDevice* m_pRD;
		static int compareElements( SGPVertex_GRASS_Cluster first, SGPVertex_GRASS_Cluster second ) noexcept;
	};
	Array<SGPVertex_GRASS_Cluster> m_GrassClusterInstanceArray;		// GrassCluster data array



	COpenGLRenderDevice*		m_pRenderDevice;
	uint32						m_GrassTextureID;			// Grass texture ID
	Vector4D					m_vCameraPos;				// current camera position


	GLuint						m_nGrassClusterVBOID;		// Grass vertex VBO
	GLuint						m_nGrassClusterIndexVBOID;	// Grass index VBO
	GLuint						m_nGrassClusterVAOID;		// Grass vertex VAO

	GLuint						m_GrassClusterInstanceVBID;	// Grass Cluster Dynamic instance VB

	Vector4D					m_vWindDirForce;			// wind params for rendering (Wind Direction And Strength)
	Vector2D					m_vTimeParams;				// time params for rendering (x is time ; y is grass movement period)
	Vector2D					m_vLightMapTextureDimision;	// terrain lightmap texture dimision (x is 1.0/width ; y is 1.0/height)
	Vector4D					m_vTextureAtlas;			// tiled texture UV params for rendering
	
	Vector3D					m_vDefaultGrassSize;		// Default grass Quad size
};

#endif