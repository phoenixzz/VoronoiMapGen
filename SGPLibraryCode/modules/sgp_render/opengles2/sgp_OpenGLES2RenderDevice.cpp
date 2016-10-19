
COpenGLES2RenderDevice::COpenGLES2RenderDevice(const SGPOpenGLES2CreationParameters& Param)
	: COpenGLES2ExtensionHandler(),
	m_pLogger(Param.plog), m_Params(Param), 
	m_AntiAlias(Param.AntiAlias), m_PixelFormat(SGPPF_A8R8G8B8),
	m_ClearRedColor(0.0f), m_ClearGreenColor(0.0f), m_ClearBlueColor(0.0f), m_ClearAlphaColor(1.0f),
	m_RenderStage(SGPRS_NORMAL),
	m_pCurrentCamera(NULL),
	m_pMTResourceLoader(NULL),
	m_pFontManager(NULL),
	m_pInstanceManager(NULL), m_pShaderManager(NULL), m_pMaterialSystem(NULL),
	m_pVertexCacheManager(NULL), m_pModelManager(NULL),	m_pParticleManager(NULL),
	m_pTextureManager(NULL),
	m_pEffectSystemManager(NULL),
	m_pWorldSystemManager(NULL),
	m_pMaterialRenderer(NULL),
	m_pTerrainRenderer(NULL),
	m_pSkydomeRenderer(NULL),
	m_pWaterRenderer(NULL),
	m_pGrassRenderer(NULL)
{
	m_pOpenGLES2Config = COpenGLES2Config::getInstance();

	OnDriverInit();

	jassert(m_pLogger);
}


COpenGLES2RenderDevice::~COpenGLES2RenderDevice()
{
	COpenGLES2Config::deleteInstance();

	if( m_pMTResourceLoader )
	{
		delete m_pMTResourceLoader;
		m_pMTResourceLoader = NULL;
	}

	if( m_pInstanceManager )
	{
		delete m_pInstanceManager;
		m_pInstanceManager = NULL;
	}

	if( m_pShaderManager )
	{
		delete m_pShaderManager;
		m_pShaderManager = NULL;
	}
	if( m_pMaterialSystem )
	{
		delete m_pMaterialSystem;
		m_pMaterialSystem = NULL;
	}
	if( m_pVertexCacheManager )
	{
		delete m_pVertexCacheManager;
		m_pVertexCacheManager = NULL;
	}
	if( m_pModelManager )
	{
		delete m_pModelManager;
		m_pModelManager = NULL;
	}
	if( m_pParticleManager )
	{
		delete m_pParticleManager;
		m_pParticleManager = NULL;
	}
	if( m_pEffectSystemManager )
	{
		delete m_pEffectSystemManager;
		m_pEffectSystemManager = NULL;
	}
	if( m_pWorldSystemManager )
	{
		delete m_pWorldSystemManager;
		m_pWorldSystemManager = NULL;
	}

	if( m_pCurrentCamera )
	{
		delete m_pCurrentCamera;
		m_pCurrentCamera = NULL;
	}

	if( m_pMaterialRenderer )
	{
		delete m_pMaterialRenderer;
		m_pMaterialRenderer = NULL;
	}

	if( m_pTerrainRenderer)
	{
		delete m_pTerrainRenderer;
		m_pTerrainRenderer = NULL;
	}

	if( m_pSkydomeRenderer )
	{
		delete m_pSkydomeRenderer;
		m_pSkydomeRenderer = NULL;
	}

	if( m_pWaterRenderer )
	{
		delete m_pWaterRenderer;
		m_pWaterRenderer = NULL;
	}

	if( m_pGrassRenderer )
	{
		delete m_pGrassRenderer;
		m_pGrassRenderer = NULL;
	}

	if( m_pFontManager )
	{
		delete m_pFontManager;
		m_pFontManager = NULL;
	}

	// Texture manager should release last
	if( m_pTextureManager )
	{
		delete m_pTextureManager;
		m_pTextureManager = NULL;
	}

}

void COpenGLES2RenderDevice::OnDriverInit()
{
	m_pOpenGLES2Config->InitConfig();
	m_pOpenGLES2Config->HandleSRGB = m_Params.HandleSRGB;
	m_pOpenGLES2Config->FullScreenAntiAlias = m_AntiAlias;



	const GLubyte* versionStr = glGetString(GL_VERSION);
	m_VideoDriverName = String("Using renderer - OpenGL ES Ver : ") + String(reinterpret_cast<const char*>(versionStr));

	m_pLogger->writeToLog(getVideoDriverName(), ELL_INFORMATION);
	
	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		m_VendorName = String( reinterpret_cast<const char*>(vendor) );
		m_pLogger->writeToLog( String(reinterpret_cast<const char*>(renderer)) + m_VendorName, ELL_INFORMATION);
	}

	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	if( glslVersion )
		m_pLogger->writeToLog(String("GLSL version : ") + String(reinterpret_cast<const char*>(glslVersion)), ELL_INFORMATION);




	// Some OPENGL ES 2.0 init setting
	// Reset The Current Viewport
	glViewport(0, 0, m_Params.WindowWidth, m_Params.WindowHeight);

	setAmbientLight( Colour(0, 0, 0, 0.0f) );

	// Init Render State
	InitOpenGLES2RenderState();


	// Manager System Init
	// Texture System
	m_pTextureManager = new CSGPTextureManager(this, m_pLogger);
	m_pTextureManager->createDefaultTexture();
	m_pTextureManager->createWhiteTexture();
	m_pTextureManager->createBlackTexture();

	// Shader System
	m_pShaderManager = new COpenGLES2ShaderManager(this, m_pLogger);

	// Camera System
	m_pCurrentCamera = new COpenGLES2Camera(this);

	// Material System
	m_pMaterialSystem = new ISGPMaterialSystem(this);
	m_pMaterialSystem->LoadGameMaterials();

	// Material Renderer
	m_pMaterialRenderer = new COpenGLES2MaterialRenderer(this);

	// Terrain Renderer
	m_pTerrainRenderer = new COpenGLES2TerrainRenderer(this);

	// Skydome Renderer
	m_pSkydomeRenderer = new COpenGLES2SkydomeRenderer(this);

	// Water Renderer
	m_pWaterRenderer = new COpenGLES2WaterRenderer(this);

	// Grass Renderer
	m_pGrassRenderer = new COpenGLES2GrassRenderer(this);

	// VertexCache System
	m_pVertexCacheManager = new COpenGLES2VertexCacheManager(this);

	// Model System
	m_pModelManager = new ISGPModelManager(this, m_pLogger);

	// Particle System
	m_pParticleManager = new ISGPParticleManager(this, m_pLogger);

	// Effect Instance System
	m_pEffectSystemManager = new ISGPEffectSystemManager(this, m_pLogger);

	// World System
	m_pWorldSystemManager = new COpenGLES2WorldSystemManager(this, m_pLogger);

	// Instance Manager
	m_pInstanceManager = new ISGPInstanceManager(this);

	// Font System
	m_pFontManager = new ISGPFontManager(this);

	// Multi-Thread Resource Loader
	m_pMTResourceLoader = new CSGPResourceLoaderMuitiThread(this);
}

void COpenGLES2RenderDevice::InitOpenGLES2RenderState()
{
	// Set the depth buffer to be entirely cleared to 1.0 values.
    glClearDepthf(1.0f);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

    // Set the polygon winding to front facing for the right handed system.
    glFrontFace(GL_CCW);

    // Enable back face culling.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}

void COpenGLES2RenderDevice::setClearColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_ClearRedColor = fRed;
	m_ClearGreenColor = fGreen;
	m_ClearBlueColor = fBlue;
	m_ClearAlphaColor = fAlpha;
}

//! init call for rendering start
bool COpenGLES2RenderDevice::beginScene( bool bClearColorBuffer, bool bClearDepthBuffer, bool bClearStencilBuffer )
{
	GLbitfield mask = 0;
	if(bClearColorBuffer)
	{
		glClearColor(m_ClearRedColor, m_ClearGreenColor, m_ClearBlueColor, m_ClearAlphaColor);
		mask |= GL_COLOR_BUFFER_BIT;
	}
	if(bClearDepthBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if(bClearStencilBuffer)
		mask |= GL_STENCIL_BUFFER_BIT;

	if(mask)
		glClear(mask);

	getOpenGLMaterialRenderer()->BeforeDrawRenderBatch();

	return true;
}

//! presents the rendered scene on the screen, returns false if failed
bool COpenGLES2RenderDevice::endScene()
{
	return true;
}

void COpenGLES2RenderDevice::clearZBuffer()
{
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void COpenGLES2RenderDevice::createRenderToFrameBuffer( uint32 Width, uint32 Height, bool stencilbuffer )
{
}

void COpenGLES2RenderDevice::recreateRenderToFrameBuffer(uint32 Width, uint32 Height, bool stencilbuffer)
{
}

void COpenGLES2RenderDevice::setRenderToFrameBuffer()
{
}

void COpenGLES2RenderDevice::renderBackToMainBuffer()
{
}

void COpenGLES2RenderDevice::deleteRenderToFrameBuffer()
{
}

void COpenGLES2RenderDevice::onResize(const uint32 width, const uint32 height)
{
	if( m_ViewPort.Width == m_ScreenSize.Width &&
		m_ViewPort.Height == m_ScreenSize.Height )
	{
		m_ViewPort.X = m_ViewPort.Y = 0;
		m_ViewPort.Width = width;
		m_ViewPort.Height = height;
	}
	m_ScreenSize.Width = width;
	m_ScreenSize.Height = height;

	setViewPort(m_ViewPort);
}

// returns the current size of the screen or rendertarget
const SDimension2D& COpenGLES2RenderDevice::getCurrentRenderTargetSize() const
{
	if (m_CurrentRTSize.Width == 0)
		return m_ScreenSize;
	else
		return m_CurrentRTSize;
}

void COpenGLES2RenderDevice::setCameraMode(SGP_CAMERAMODE_TYPE Mode)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->setCameraMode(Mode);
}

void COpenGLES2RenderDevice::setFov(float fov)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->m_fFov = fov;
}

void COpenGLES2RenderDevice::setViewPort(const SViewPort& area)
{
	m_ViewPort.X = area.X;
	m_ViewPort.Y = area.Y;
	m_ViewPort.Width = area.Width;
	m_ViewPort.Height = area.Height;

	setProjMatrixParams((float)m_ViewPort.Width/(float)m_ViewPort.Height);
			
	glViewport(m_ViewPort.X, m_ViewPort.Y, m_ViewPort.Width, m_ViewPort.Height);
}

void COpenGLES2RenderDevice::setNearFarClipPlane(float fNear, float fFar)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->SetClippingPlanes( fNear, fFar );
}

/**
 * Calculate perspective and orthogonal projection matrix for current camera
 * using given values.
 * -> IN: float       - field of view (horizontal)(in degree)
 *        float		  - aspect ration (viewport height / width)
 */
void COpenGLES2RenderDevice::setProjMatrixParams(float fAspect)
{
	if(!m_pCurrentCamera)
		return;

	if( m_pCurrentCamera->m_Mode == SGPCT_PERSPECTIVE )
	{
		// PERSPECTIVE PROJECTION MATRIX
		m_pCurrentCamera->CalcPerspProjMatrix(fAspect);
	}
	else if( m_pCurrentCamera->m_Mode == SGPCT_ORTHOGONAL )
	{
		m_pCurrentCamera->CalcOrthoProjMatrix(
			(float)m_ViewPort.X, (float)m_ViewPort.X+m_ViewPort.Width,
			(float)m_ViewPort.Y, (float)m_ViewPort.Y+m_ViewPort.Height,
			m_pCurrentCamera->m_fNear,
			m_pCurrentCamera->m_fFar );
	}
	else if( m_pCurrentCamera->m_Mode == SGPCT_TWOD )
	{
		m_pCurrentCamera->Prepare2DMode();
	}
}

void COpenGLES2RenderDevice::setViewMatrix3D( 
	const Vector4D& vcRight, const Vector4D& vcUp, const Vector4D& vcDir, const Vector4D& vcEyePos )
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->SetViewMatrix3D(vcRight, vcUp, vcDir, vcEyePos);
}

/**
 * Calculates current camera View matrix for given point and calls SetViewMatrix3D
 * to active the look at matrix.
 * -> IN: Vector4D& - 4 floats building camera's position vector
 *        Vector4D& - 4 floats building lookat point vector
 *        Vector4D& - 4 floats building world up vector at cam pos
 */
void COpenGLES2RenderDevice::setViewMatrixLookAt( 
	const Vector4D& vcPos, const Vector4D& vcPoint, const Vector4D& vcWorldUp) 
{
	Vector4D vcDir, vcTemp, vcUp;

	vcDir = vcPoint - vcPos;
	vcDir.Normalize();

	// calculate up vector
	float fDot = vcWorldUp * vcDir;
 	vcTemp = vcDir * fDot;
	vcUp = vcWorldUp - vcTemp;
	float fL = vcUp.GetLength();

	// if length too small take normal y axis as up vector
	if(fL < 1e-6f) 
	{
		Vector4D vcY;
		vcY.Set(0.0f, 1.0f, 0.0f);

		vcTemp = vcDir * vcDir.y;
		vcUp = vcY - vcTemp;

		fL = vcUp.GetLength();

		// if still too small take z axis as up vector
		if (fL < 1e-6f) 
		{
			vcY.Set(0.0f, 0.0f, 1.0f);

			vcTemp = vcDir * vcDir.z;
			vcUp = vcY - vcTemp;

			// if still too small we are lost         
			fL = vcUp.GetLength();
			if(fL < 1e-6f) 
				return;
		}
	}

	vcUp /= fL;

	// build right vector using cross product
	Vector4D vcRight;
	vcRight.Cross(vcUp, vcDir);
	vcRight.Normalize();

	vcUp.Cross(vcDir, vcRight);

	return setViewMatrix3D(vcRight, vcUp, vcDir, vcPos);
}

void COpenGLES2RenderDevice::getCamreaPosition(Vector4D* pCameraPos)
{
	if( m_pCurrentCamera && pCameraPos )
	{
		pCameraPos->Set(m_pCurrentCamera->GetPos());
	}
}

void COpenGLES2RenderDevice::getCamreaViewDirection(Vector3D* pViewDir)
{
	if( m_pCurrentCamera && pViewDir )
	{
		pViewDir->Set(m_pCurrentCamera->m_mViewMatrix._13, m_pCurrentCamera->m_mViewMatrix._23, m_pCurrentCamera->m_mViewMatrix._33);
	}
}

void COpenGLES2RenderDevice::getViewFrustrum(Plane *pPlane)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetFrustrum(pPlane);
}

void COpenGLES2RenderDevice::getViewMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetViewMatrix(mat);
}

void COpenGLES2RenderDevice::getProjMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetProjMatrix(mat);
}

void COpenGLES2RenderDevice::getViewProjMatrix(Matrix4x4& mat)
{
	if( m_pCurrentCamera )
		m_pCurrentCamera->GetViewProjMatrix(mat);
}

/**
 * Cast a world ray from a given position on screen.
 * -> IN:  POINT       - point in screen coordinates
 * -> OUT: Vector4D& - to fill with 4 floats for ray origin vector
 *         Vector4D& - to fill with 4 floats for ray direction
 */
void COpenGLES2RenderDevice::Transform2Dto3D(const SDimension2D &pt, Vector4D *vcOrig, Vector4D *vcDir)
{
	Matrix4x4 *pView = NULL;
	Matrix4x4 *pProj = NULL;
	Matrix4x4 mInvView;
	Vector4D vcS;
	uint32 dwWidth, dwHeight;

	if( m_pCurrentCamera )
	{
		dwWidth  = m_ViewPort.Width;
		dwHeight = m_ViewPort.Height;

		pView = &m_pCurrentCamera->m_mViewMatrix;
		pProj = &m_pCurrentCamera->m_mProjMatrix;

		// resize to viewportspace [-1,1] -> projection
		vcS.x =  ( ((pt.Width*2.0f) / dwWidth) -1.0f) / pProj->_11;
		vcS.y = -( ((pt.Height*2.0f) / dwHeight)-1.0f) / pProj->_22;
		vcS.z = 1.0f;

		// invert view matrix
		mInvView.InverseOf(*pView);

		// ray from screen to worldspace
		(*vcDir).x = (vcS.x * mInvView._11)	+ (vcS.y * mInvView._21) + (vcS.z * mInvView._31);
		(*vcDir).y = (vcS.x * mInvView._12)	+ (vcS.y * mInvView._22) + (vcS.z * mInvView._32);
		(*vcDir).z = (vcS.x * mInvView._13)	+ (vcS.y * mInvView._23) + (vcS.z * mInvView._33);
   
		// inverse translation.
		(*vcOrig).x = mInvView._41;
		(*vcOrig).y = mInvView._42;
		(*vcOrig).z = mInvView._43;

		// normalize
		(*vcDir).Normalize();
	}
}

/**
 * Cast given point from world space to screen space.
 * -> IN:     Vector4D		- vector to position in 3d space 
 * -> RETURN: SDimension2D  - corresponding point in 2d screen space
 */
SDimension2D COpenGLES2RenderDevice::Transform3Dto2D(const Vector4D &vcPoint)
{
	SDimension2D pt;
	pt.Width = pt.Height = 0;
	float fClip_x, fClip_y;
	float fXp, fYp, fWp;
	uint32 dwWidth, dwHeight;

	if( m_pCurrentCamera )
	{
		dwWidth  = m_ViewPort.Width;
		dwHeight = m_ViewPort.Height;

		fClip_x = (float)(dwWidth  >> 1);
		fClip_y = (float)(dwHeight >> 1);

		fXp =	(m_pCurrentCamera->m_mViewProjMatrix._11*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._21*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._31*vcPoint.z) + 
				(m_pCurrentCamera->m_mViewProjMatrix._41);
		fYp =	(m_pCurrentCamera->m_mViewProjMatrix._12*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._22*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._32*vcPoint.z) +
				(m_pCurrentCamera->m_mViewProjMatrix._42);
		fWp =	(m_pCurrentCamera->m_mViewProjMatrix._14*vcPoint.x) +
				(m_pCurrentCamera->m_mViewProjMatrix._24*vcPoint.y) +
				(m_pCurrentCamera->m_mViewProjMatrix._34*vcPoint.z) + 
				(m_pCurrentCamera->m_mViewProjMatrix._44);

		float fWpInv = 1.0f / fWp;

		// transform from [-1,1] to actual viewport dimensions
		pt.Width = (uint32)( (1.0f + (fXp * fWpInv)) * fClip_x );
		pt.Height = (uint32)( (1.0f + (fYp * fWpInv)) * fClip_y );
	}

	return pt;
}

void COpenGLES2RenderDevice::FlushRenderBatch()
{
	// Commit Dynamic Buffer
	GetVertexCacheManager()->ForcedCommitAll();

	getOpenGLMaterialRenderer()->QueueRenderBatch();

	// STEP 1 : Render water reflection
	if( GetWorldSystemManager()->needRenderWater() )
	{
		// Switch to Render Water Reflection Render Target
		getOpenGLWaterRenderer()->discardFramebuffer(false, false);
		getOpenGLWaterRenderer()->switchToReflectionRenderTarget(true, true, false);		

		// do draw Water Reflection Map
		m_RenderStage = SGPRS_WATERREFLECTION;
		getOpenGLWaterRenderer()->DoDrawWaterReflectionMap();

		// Switch to Render Water Scene Buffer Render Target( Reflection Scene Depth and Stencil will be unuseful )
		getOpenGLWaterRenderer()->discardFramebuffer(true, true);
		getOpenGLWaterRenderer()->switchToSceneBufferRenderTarget(true, true, false);	

		// Reset to Normal renderer
		m_RenderStage = SGPRS_NORMAL;
		// Clear Shader Program ID cache
		ISGPRenderBatch::m_ShaderProgramIdx = -1;
		COpenGLSLES2ShaderProgram::m_CurrentProgramID = 0;
	}

	// STEP 2 : Skydome
	getOpenGLSkydomeRenderer()->DoDrawSkydomeRenderBatch();

	// STEP 3 : Terrain
	getOpenGLTerrainRenderer()->DoDrawTerrainRenderBatch();

	// STEP 4 : Opaque Renderbatch
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_Opaque();

	// STEP 5 : Skin Animination
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnim();

	// STEP 6 : Alpha-Test Renderbatch
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_AlphaTest();

	// STEP 7 : Alpha-Test Skin Animination 
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnimAlphaTest();



	// STEP 8 : water refraction and water render
	if( GetWorldSystemManager()->needRenderWater() )
	{
		// First Render water Refraction Mask
		getOpenGLWaterRenderer()->DoDrawWaterRefractionMap();

		// Switch to Water Refraction map Render Target
		getOpenGLWaterRenderer()->discardFramebuffer(false, false);
		getOpenGLWaterRenderer()->switchToRefractionRenderTarget(true, false, false);

		// Render Full screen Quad to copy Water SceneBuffer color ==> Water Refraction RT buffer
		getOpenGLWaterRenderer()->renderSceneBufferFullScreenQuad();

		// Switch back to Water SceneBuffer RT (Refraction Scene Depth and Stencil will be unuseful)
		getOpenGLWaterRenderer()->discardFramebuffer(true, true);
		getOpenGLWaterRenderer()->switchToSceneBufferRenderTarget(false, false, false);

		// Render Final Water Effect
		getOpenGLWaterRenderer()->DoDrawWaterSimulation(getOpenGLWaterRenderer()->getReflectionMapID(), getOpenGLWaterRenderer()->getRefractionMapID());
	
		// Clear Shader Program ID cache
		ISGPRenderBatch::m_ShaderProgramIdx = -1;	
		COpenGLSLES2ShaderProgram::m_CurrentProgramID = 0;
	}

	getOpenGLTerrainRenderer()->AfterDrawTerrainRenderBatch();

	// STEP 9 : grass
	getOpenGLGrassRenderer()->DoDrawGrass();

	// STEP 10 : Transparent Renderbatch
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_Transparent();

	// STEP 11 : SkinAnim with alpha Renderbatch
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnimAlpha();

	// STEP 12 : Particle
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_Particle();
	
	// STEP 13 : Debug info
	getOpenGLMaterialRenderer()->DoDrawRenderBatch_DebugLine();
	
	// Below Step, Buffer Depth info will disappear
	// STEP 14 : Copy back color buffer from water renderer
	if( GetWorldSystemManager()->needRenderWater() )
	{
		// Render Full screen Quad to copy Water SceneBuffer color ==> OpenGL back buffer
		getOpenGLWaterRenderer()->discardFramebuffer(true, true);
		getOpenGLWaterRenderer()->switchSceneBufferToBackBuffer(false, false, false);
		getOpenGLWaterRenderer()->renderSceneBufferFullScreenQuad();
	}

	// STEP 15 : Post Process
	{
	}

	getOpenGLMaterialRenderer()->AfterDrawRenderBatch();	

	// Clear Dynamic Buffer
	GetVertexCacheManager()->ForcedClearAll();

	// Clear Shader Program ID cache
	ISGPRenderBatch::m_ShaderProgramIdx = -1;
	COpenGLSLES2ShaderProgram::m_CurrentProgramID = 0;
}

SGP_TTFFont* COpenGLES2RenderDevice::CreateTTFFont(FT_Library& FTLib)
{
	return new SGP_OpenGLES2TTFFont(FTLib, this);
}

void COpenGLES2RenderDevice::BeginRenderText()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &FontMaterial_info = Mat_List[ISGPMaterialSystem::eMaterial_font];
	getOpenGLMaterialRenderer()->PushMaterial(FontMaterial_info.m_material, MM_Add);
	getOpenGLMaterialRenderer()->ComputeMaterialPass();
	getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);
}

void COpenGLES2RenderDevice::EndRenderText()
{
	m_pFontManager->FlushAllFonts();

	getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	getOpenGLMaterialRenderer()->PopMaterial();
}

void COpenGLES2RenderDevice::SetActiveFont(const char* FontName)
{
	m_pFontManager->SetActiveFont( FontName );
}

bool COpenGLES2RenderDevice::CreateFontInManager(const char* FontName, const String& FontFilePath, bool bBold, bool bItalic, uint16 iSize)
{
	String FontAbsolutePath = FontFilePath;
	// Identify font file by their absolute filenames if possible.
	if( !File::isAbsolutePath(FontAbsolutePath) )
	{
		FontAbsolutePath = m_WorkingDir + File::separatorString + FontFilePath;
	}

	if( m_pFontManager->IsFontLoaded(FontName) )
		return true;

	if( !m_pFontManager->AddFont( FontName, FontAbsolutePath, iSize, bBold, bItalic ) )
		return false;

	m_pFontManager->SetActiveFont( FontName );

	return true;
}

void COpenGLES2RenderDevice::DrawTextInPos(int xPos, int yPos, uint32 flag, float RealFontSize, uint8 r, uint8 g, uint8 b, wchar_t* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	int Length = vswprintf(0, 0, format, ArgPtr)/*_vscwprintf(format, ArgPtr)*/ + 1;
	if( Length > MAX_FONT_STRING )
	{
		m_pLogger->writeToLog(String("Render text buffer overflow!"), ELL_ERROR);
		return;
	}
	vswprintf(m_FontString, Length, format, ArgPtr);
	va_end(ArgPtr);

	Colour FontColor( r, g, b );
	float screenPos_x = static_cast<float>(xPos);
	float screenPos_y = static_cast<float>(yPos);

	m_pFontManager->DoDrawTextInPos(String(m_FontString), screenPos_x, screenPos_y, FontColor, flag, RealFontSize, false, (flag & SGPFDL_UNDERLINE) ?  true : false);
}

void COpenGLES2RenderDevice::DrawTextAlignWrap(int xPos, int yPos, uint32 flag, float RealFontSize, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align, uint8 r, uint8 g, uint8 b, wchar_t* format, ...)
{
	// TODO
}

void COpenGLES2RenderDevice::PreCacheChar(const String& sText)
{
	m_pFontManager->PreCacheChar(sText);
}

void COpenGLES2RenderDevice::setWorkingDirection(const String& workingDir)
{
	m_WorkingDir = workingDir;
	if( m_pTextureManager )
		m_pTextureManager->setWorkingDirection(m_WorkingDir);
	if( m_pModelManager )
		m_pModelManager->setWorkingDirection(m_WorkingDir);
	if( m_pParticleManager )
		m_pParticleManager->setWorkingDirection(m_WorkingDir);
	if( m_pEffectSystemManager )
		m_pEffectSystemManager->setWorkingDirection(m_WorkingDir);
}

ISGPTexture* COpenGLES2RenderDevice::createTexture( ISGPImage* pImage, 
	const String& AbsolutePath, bool bGenMipMap )
{
	return new COpenGLES2Texture(pImage, AbsolutePath, this, bGenMipMap);
}

AbstractMaterial* COpenGLES2RenderDevice::createMaterial()
{
	return new OpenGLES2Material();
}

AbstractMaterial* COpenGLES2RenderDevice::createMaterial(char* MaterialStr)
{
	return new OpenGLES2Material(MaterialStr);
}

// Creates one new ISGPParticleSystem from Particle setting
uint32 COpenGLES2RenderDevice::createOpenGLParticleSystem(const SGPMF1ParticleTag& PartSetting)
{	

	Matrix4x4 ParticleWorldMatrix;
	ParticleWorldMatrix.Identity();
	uint32 ParticleID = GetParticleManager()->createParticleSystem(ParticleWorldMatrix);
	ISGPParticleSystem* pSystem = GetParticleManager()->getParticleSystemByID( ParticleID );
	
	SPARK::Group** pParticleGroups = pSystem->createParticleGroups(PartSetting);
	if( pParticleGroups )
	{
		for(uint32 i=0; i<PartSetting.m_SystemParam.m_groupCount; ++i)
			pSystem->addGroup( pParticleGroups[i] );

		delete [] pParticleGroups;
		pParticleGroups = NULL;
	}
	pSystem->enableAABBComputing(PartSetting.m_SystemParam.m_bEnableAABBCompute);

	return ParticleID;
}

// Creates one new OpenGL Particle Renderer from ParticleRenderParam setting
SPARK::Renderer* COpenGLES2RenderDevice::createOpenGLParticleRenderer( const ParticleRenderParam& renderParam )
{
	switch(renderParam.m_type)
	{
	case Render_Quad:
		{
			const ParticleQuadRenderData& quadData = renderParam.m_quadData;
			SPARKOpenGLES2QuadRenderer* quadRender = SPARKOpenGLES2QuadRenderer::create( this, quadData.m_xScale, quadData.m_yScale );
			quadRender->setTexture( GetTextureManager()->getTextureIDByName(String(quadData.m_texPath)) );
			quadRender->setTexturingMode( (SPARK::TexturingMode)quadData.m_texMode );
			quadRender->setAtlasDimensions( quadData.m_xDimension, quadData.m_yDimension );
			quadRender->setOrientation( (SPARK::OrientationPreset)quadData.m_nOrientation );
			if(quadData.m_nOrientation == SPARK::AROUND_AXIS)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
			}
			else if(quadData.m_nOrientation == SPARK::TOWARDS_POINT)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
			}
			else if(quadData.m_nOrientation == SPARK::FIXED_ORIENTATION)
			{
				quadRender->lookVector.x = quadData.m_lookVector[0];
				quadRender->lookVector.y = quadData.m_lookVector[1];
				quadRender->lookVector.z = quadData.m_lookVector[2];
				quadRender->upVector.x = quadData.m_upVector[0];
				quadRender->upVector.y = quadData.m_upVector[1];
				quadRender->upVector.z = quadData.m_upVector[2];
			}
			return quadRender;
		}
		break;
	case Render_Line:
		{
			const ParticleLineRenderData& lineData = renderParam.m_lineData;
			SPARKOpenGLES2LineRenderer* lineRender = SPARKOpenGLES2LineRenderer::create( this, lineData.m_length, lineData.m_width );
			return lineRender;
		}
		break;
	case Render_Point:
		{
			const ParticlePointRenderData& pointData = renderParam.m_pointData;
			SPARKOpenGLES2PointRenderer* pointRender = SPARKOpenGLES2PointRenderer::create( this, pointData.m_size );
			pointRender->setType( (SPARK::PointType)pointData.m_type );
			pointRender->setTexture( GetTextureManager()->getTextureIDByName(String(pointData.m_texPath)) );
			return pointRender;
		}
		break;
	}
	return NULL;
}

//! Returns an image created from the last rendered frame.
ISGPImage* COpenGLES2RenderDevice::createScreenShot(SGP_PIXEL_FORMAT format)
{
	ISGPImage* newImage = 0;

	return newImage;
}
















//! Convert SGP_PRIMITIVE_TYPE to OpenGL equivalent
GLenum COpenGLES2RenderDevice::primitiveTypeToGL(SGP_PRIMITIVE_TYPE type) const
{
	switch (type)
	{
		case SGPPT_POINTS:
			return GL_POINTS;
		case SGPPT_LINE_STRIP:
			return GL_LINE_STRIP;
		case SGPPT_LINE_LOOP:
			return GL_LINE_LOOP;
		case SGPPT_LINES:
			return GL_LINES;
		case SGPPT_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case SGPPT_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		case SGPPT_TRIANGLES:
			return GL_TRIANGLES;
	}
	return GL_TRIANGLES;
}

//! prints error if an error happened.
bool COpenGLES2RenderDevice::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch (g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		m_pLogger->writeToLog(String("GL_INVALID_ENUM"), ELL_ERROR); break;
	case GL_INVALID_VALUE:
		m_pLogger->writeToLog(String("GL_INVALID_VALUE"), ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		m_pLogger->writeToLog(String("GL_INVALID_OPERATION"), ELL_ERROR); break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		m_pLogger->writeToLog(String("GL_INVALID_FRAMEBUFFER_OPERATION"), ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		m_pLogger->writeToLog(String("GL_OUT_OF_MEMORY"), ELL_ERROR); break;

	};
	jassertfalse;
	return true;
#else
	return false;
#endif
}