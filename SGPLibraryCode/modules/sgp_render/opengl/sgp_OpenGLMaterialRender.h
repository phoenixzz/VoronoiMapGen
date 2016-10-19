#ifndef __SGP_OPENGLMATERIALRENDER_HEADER__
#define __SGP_OPENGLMATERIALRENDER_HEADER__



// MaterialRenderer
class COpenGLMaterialRenderer
{
public :
	COpenGLMaterialRenderer(COpenGLRenderDevice *pRenderDevice);
	~COpenGLMaterialRenderer();


	// Init & Interface
	inline COpenGLRenderDevice*	GetOpenGLRenderDevice()	{ return m_pRenderDevice; };


	// Push/Pop material
	inline void PushMaterial( AbstractMaterial * material, SGP_MATERIAL_MERGE mode = MM_Add );
	inline void PopMaterial();

	// Separate Setting Render material according to pass
	void		OnePassPreRenderMaterial( int pass );
	void		OnePassPostRenderMaterial( int pass );
	int			ComputeMaterialPass();		// return how many passes in this material


	// Render Batch
	void		BeforeDrawRenderBatch();	// Do something before build Render batch lists
	void		AfterDrawRenderBatch();		// Do something after rendering
	void		QueueRenderBatch();			// Queue batch list according value

	void		DoDrawRenderBatch_Opaque();			// Render opaque render batch
	void		DoDrawRenderBatch_Transparent();	// Render Transparent render batch
	void		DoDrawRenderBatch_DebugLine();		// Render DEBUG Lines render batch
	void		DoDrawRenderBatch_SkinAnim();		// Render skeleton GPU Skin Anim render batch
	void		DoDrawRenderBatch_SkinAnimAlpha();	// Render AVMesh skeleton GPU Skin Anim render batch
	void		DoDrawRenderBatch_Particle();		// Render Particle (point sprites/Line/Quad) render batch
	void		DoDrawRenderBatch_NoDepthline();	// Render NoDepth Lines render batch ( usually for Editor )

	void		PushOpaqueRenderBatch(const COpaqueRenderBatch& batch);
	void		PushTransparentRenderBatch(const CTransparentRenderBatch& batch);
	void		PushParticlePSRenderBatch(const CParticlePSRenderBatch& batch);
	void		PushParticleLineRenderBatch(const CParticleLineRenderBatch& batch);
	void		PushParticleQuadRenderBatch(const CParticleQuadRenderBatch& batch);
	void		PushLineRenderBatch(const CLineRenderBatch& batch);
	void		PushSkinAnimRenderBatch(const CSkinAnimRenderBatch& batch);

	//void		CommitAdditionalMaterial(const struct ISGPMaterialSystem::SGPMaterialInfo& MT, const struct SGPSkin& Skin );

public:
	OpaqueRenderBatchVector				m_opaqueBatch;
	OpaqueRenderBatchVector				m_opaqueLightMapBatch;
	TransparentRenderBatchVector		m_transparentBatch;
	TransparentRenderBatchVector		m_transparentLightMapBatch;
	ParticlePSRenderBatchVector			m_particlePSBatch;
	ParticleLineRenderBatchVector		m_particleLineBatch;
	ParticleQuadRenderBatchVector		m_particleQuadBatch;
	LineRenderBatchVector				m_lineBatch;
	SkinAnimRenderBatchVector			m_skinanimBatch;
	SkinAnimRenderBatchVector			m_skinanimAlphaBatch;

	OpaqueRenderBatchVector				m_alphaTestBatch;
	OpaqueRenderBatchVector				m_alphaTestLightMapBatch;
	SkinAnimRenderBatchVector			m_skinanimAlphaTestBatch;

private:
	static const int INIT_MATERIALINFO_ARRAY_NUM = 64;
	static const int INIT_FEW_RENDERBATCH_ARRAY_NUM = 4;
	static const int INIT_SMALL_RENDERBATCH_ARRAY_NUM = 256;
	static const int INIT_LARGE_RENDERBATCH_ARRAY_NUM = 1024;

	// ReAlloc RenderBatch Resource
	ISGPRenderBatch* GetOneFreeRenderBatchFromPool(ISGPRenderBatch::BatchType BTtype);
	void ReAllocRenderBatchPool();

	// materials & drawing
	inline void Draw( ISGPRenderBatch::BatchType BTtype, AbstractMaterial * material, SGP_MATERIAL_MERGE mode = MM_Add );
	
	void Draw( ISGPRenderBatch::BatchType BTtype );
	void FastDraw( ISGPRenderBatch::BatchType BTtype );
	void SlowDraw( ISGPRenderBatch::BatchType BTtype );
	void RenderBatchDrawCall( ISGPRenderBatch::BatchType BTtype );

	struct Sorter
    {
		static int compareElements( ISGPRenderBatch* first, ISGPRenderBatch* second ) noexcept
		{
			if( first->GetQueueValue() < second->GetQueueValue() )
				return -1;
			if( first->GetQueueValue() > second->GetQueueValue() )
				return 1;
			return 0;
		}
	};

	// private types
	struct MaterialInfo
	{
		MaterialInfo() {}
		MaterialInfo( AbstractMaterial * material, SGP_MATERIAL_MERGE mode ) : material_( material ), mode_( mode ) {}

		AbstractMaterial * material_;
		SGP_MATERIAL_MERGE mode_;
		int firstPass_;
		int lastPass_;
	};

	Array<MaterialInfo>				materialStack_;
	COpenGLRenderDevice*			m_pRenderDevice;
	int								m_FirstMaterial;

	OwnedArrayOpaqueRenderBatch				m_OpaqueRenderBatchPool;
	OwnedArrayTransparentRenderBatch		m_TransparentRenderBatchPool;
	OwnedArrayParticlePSRenderBatch			m_ParticlePSRenderBatchPool;
	OwnedArrayParticleLineRenderBatch		m_ParticleLineRenderBatchPool;
	OwnedArrayParticleQuadRenderBatch		m_ParticleQuadRenderBatchPool;
	OwnedArrayLineRenderBatch				m_LineRenderBatchPool;
	OwnedArraySkinAnimRenderBatch			m_SkinAnimRenderBatchPool;
	static int m_OpaqueRenderBatchPoolUsed;
	static int m_TransparentRenderBatchPoolUsed;
	static int m_ParticlePSRenderBatchPoolUsed;
	static int m_ParticleLineRenderBatchPoolUsed;
	static int m_ParticleQuadRenderBatchPoolUsed;
	static int m_LineRenderBatchPoolUsed;
	static int m_SkinAnimRenderBatchPoolUsed;
};


// inlines
inline void COpenGLMaterialRenderer::PushMaterial( AbstractMaterial * material, SGP_MATERIAL_MERGE mode )
{
	materialStack_.add( MaterialInfo( material, mode ) );
}

inline void COpenGLMaterialRenderer::PopMaterial()
{
	materialStack_.removeLast();
}

inline void COpenGLMaterialRenderer::Draw( ISGPRenderBatch::BatchType BTtype, AbstractMaterial * material, SGP_MATERIAL_MERGE mode )
{
	materialStack_.add( MaterialInfo( material, mode ) );
	Draw( BTtype );
	materialStack_.removeLast();
}

#endif		// __SGP_OPENGLMATERIALRENDER_HEADER__