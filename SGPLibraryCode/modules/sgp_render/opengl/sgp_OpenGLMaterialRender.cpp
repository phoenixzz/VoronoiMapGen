int COpenGLMaterialRenderer::m_OpaqueRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_TransparentRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_ParticlePSRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_ParticleLineRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_ParticleQuadRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_LineRenderBatchPoolUsed = 0;
int COpenGLMaterialRenderer::m_SkinAnimRenderBatchPoolUsed = 0;


COpenGLMaterialRenderer::COpenGLMaterialRenderer(COpenGLRenderDevice *pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_FirstMaterial(0)
{
	materialStack_.ensureStorageAllocated(INIT_MATERIALINFO_ARRAY_NUM);

	m_opaqueBatch.ensureStorageAllocated(INIT_LARGE_RENDERBATCH_ARRAY_NUM);
	m_opaqueLightMapBatch.ensureStorageAllocated(INIT_LARGE_RENDERBATCH_ARRAY_NUM);
	m_transparentBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_transparentLightMapBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_particlePSBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_particleLineBatch.ensureStorageAllocated(INIT_FEW_RENDERBATCH_ARRAY_NUM);
	m_particleQuadBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_lineBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_skinanimBatch.ensureStorageAllocated(INIT_LARGE_RENDERBATCH_ARRAY_NUM);
	m_skinanimAlphaBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);

	m_alphaTestBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_alphaTestLightMapBatch.ensureStorageAllocated(INIT_SMALL_RENDERBATCH_ARRAY_NUM);
	m_skinanimAlphaTestBatch.ensureStorageAllocated(INIT_LARGE_RENDERBATCH_ARRAY_NUM);

	ReAllocRenderBatchPool();
}

COpenGLMaterialRenderer::~COpenGLMaterialRenderer()
{
}

int	COpenGLMaterialRenderer::ComputeMaterialPass()
{
	// compute the first active material
	m_FirstMaterial = materialStack_.size() - 1;
	for( int i = 0; i < m_FirstMaterial; ++i )
	{
		if( materialStack_.getReference(i).mode_ == MM_Replace )
		{
			m_FirstMaterial = i;
			break;
		}
	}

	// compute the first & last pass for each active material
	int currentPass = 0;
	for( int i = m_FirstMaterial; i >= 0; --i )
	{
		MaterialInfo & mat = materialStack_.getReference(i);

		switch( mat.mode_ )
		{
		case MM_Add :
		case MM_Replace :
			mat.firstPass_ = currentPass;
			currentPass += mat.material_->GetPassCount();
			mat.lastPass_ = currentPass - 1;
			break;

		case MM_Blend :
			mat.firstPass_ = 0;
			mat.lastPass_ = currentPass - 1;
			break;
		}
	}
	return currentPass;
}

void COpenGLMaterialRenderer::OnePassPreRenderMaterial(int pass)
{
	// pre render
	for( int j = m_FirstMaterial; j >= 0; --j )
	{
		const MaterialInfo & mat = materialStack_.getReference(j);
		if( pass >= mat.firstPass_ && pass <= mat.lastPass_ )
			mat.material_->PreRender( pass % mat.material_->GetPassCount() );
	}
}

void COpenGLMaterialRenderer::OnePassPostRenderMaterial(int pass)
{
	// post render
	for( int j = m_FirstMaterial; j >= 0; --j )
	{
		const MaterialInfo & mat = materialStack_.getReference(j);
		if( pass >= mat.firstPass_ && pass <= mat.lastPass_ )
			mat.material_->PostRender( pass % mat.material_->GetPassCount() );
	}
}

void COpenGLMaterialRenderer::Draw( ISGPRenderBatch::BatchType BTtype )
{
	switch( materialStack_.size() )
	{
	case 0 :
		// empty stack: nothing to draw
		break;

	case 1 :
		// one material: fast draw
		FastDraw( BTtype );
		break;

	default :
		// multiple materials: slow draw
		SlowDraw( BTtype );
		break;
	}
}



void COpenGLMaterialRenderer::FastDraw( ISGPRenderBatch::BatchType BTtype )
{
	// render the object using a single material
	AbstractMaterial * material = materialStack_.getReference(0).material_;
	if( material )
	{
		const int numPass = material->GetPassCount();
		for( int i = 0; i < numPass; ++i )
		{
			material->PreRender( i );

			RenderBatchDrawCall(BTtype);

			material->PostRender( i );
		}
	}
}



void COpenGLMaterialRenderer::SlowDraw( ISGPRenderBatch::BatchType BTtype )
{
	// compute the first active material
	int firstMaterial = materialStack_.size() - 1;
	for( int i = 0; i < firstMaterial; ++i )
	{
		if( materialStack_[i].mode_ == MM_Replace )
		{
			firstMaterial = i;
			break;
		}
	}

	// compute the first & last pass for each active material
	int currentPass = 0;
	for( int i = firstMaterial; i >= 0; --i )
	{
		MaterialInfo & mat = materialStack_.getReference(i);

		switch( mat.mode_ )
		{
		case MM_Add :
		case MM_Replace :
			mat.firstPass_ = currentPass;
			currentPass += mat.material_->GetPassCount();
			mat.lastPass_ = currentPass - 1;
			break;

		case MM_Blend :
			mat.firstPass_ = 0;
			mat.lastPass_ = currentPass - 1;
			break;
		}
	}

	// render loop
	for( int i = 0; i < currentPass; ++i )
	{
		// pre render
		for( int j = firstMaterial; j >= 0; --j )
		{
			const MaterialInfo & mat = materialStack_.getReference(j);
			if( i >= mat.firstPass_ && i <= mat.lastPass_ )
				mat.material_->PreRender( i % mat.material_->GetPassCount() );
		}

		RenderBatchDrawCall(BTtype);

		for( int j = firstMaterial; j >= 0; --j )
		{
			const MaterialInfo & mat = materialStack_.getReference(j);
			if( i >= mat.firstPass_ && i <= mat.lastPass_ )
				mat.material_->PostRender( i % mat.material_->GetPassCount() );
		}
	}
}

void COpenGLMaterialRenderer::RenderBatchDrawCall( ISGPRenderBatch::BatchType BTtype )
{
	switch( BTtype )
	{
	case ISGPRenderBatch::eBatchOpaque:
		{
			for( int i= 0; i<m_opaqueBatch.size(); i++ )
			{
				m_opaqueBatch[i]->PreRender();
				m_opaqueBatch[i]->Render();
				m_opaqueBatch[i]->PostRender();
			}
			for( int i= 0; i<m_alphaTestBatch.size(); i++ )
			{
				m_alphaTestBatch[i]->PreRender();
				m_alphaTestBatch[i]->Render();
				m_alphaTestBatch[i]->PostRender();
			}
			for( int i= 0; i<m_opaqueLightMapBatch.size(); i++ )
			{
				m_opaqueLightMapBatch[i]->PreRender();
				m_opaqueLightMapBatch[i]->Render();
				m_opaqueLightMapBatch[i]->PostRender();
			}
			for( int i= 0; i<m_alphaTestLightMapBatch.size(); i++ )
			{
				m_alphaTestLightMapBatch[i]->PreRender();
				m_alphaTestLightMapBatch[i]->Render();
				m_alphaTestLightMapBatch[i]->PostRender();
			}
		}
		break;
	case ISGPRenderBatch::eBatchTransparent:
		{
			for( int i= 0; i<m_transparentBatch.size(); i++ )
			{
				m_transparentBatch[i]->PreRender();
				m_transparentBatch[i]->Render();
				m_transparentBatch[i]->PostRender();
			}
			for( int i= 0; i<m_transparentLightMapBatch.size(); i++ )
			{
				m_transparentLightMapBatch[i]->PreRender();
				m_transparentLightMapBatch[i]->Render();
				m_transparentLightMapBatch[i]->PostRender();
			}
		}
		break;
	case ISGPRenderBatch::eBatchParticlePointSprites:
		{
			for( int i= 0; i<m_particlePSBatch.size(); i++ )
			{
				m_particlePSBatch[i]->PreRender();
				m_particlePSBatch[i]->Render();
				m_particlePSBatch[i]->PostRender();
			}
		}
		break;
	case ISGPRenderBatch::eBatchParticleLine:
		{
			for( int i= 0; i<m_particleLineBatch.size(); i++ )
			{
				m_particleLineBatch[i]->PreRender();
				m_particleLineBatch[i]->Render();
				m_particleLineBatch[i]->PostRender();
			}
		}
		break;
	case ISGPRenderBatch::eBatchParticleQuad:
		{
			for( int i= 0; i<m_particleQuadBatch.size(); i++ )
			{
				m_particleQuadBatch[i]->PreRender();
				m_particleQuadBatch[i]->Render();
				m_particleQuadBatch[i]->PostRender();
			}
		}
		break;
	case ISGPRenderBatch::eBatchSkinAnim:
		{
			ISGPRenderBatch::m_SkinAnimShaderLightmapTexSetted = false;
			for( int i= 0; i<m_skinanimBatch.size(); i++ )
			{
				m_skinanimBatch[i]->PreRender();
				m_skinanimBatch[i]->Render();
				m_skinanimBatch[i]->PostRender();
			}
			for( int i= 0; i<m_skinanimAlphaTestBatch.size(); i++ )
			{
				m_skinanimAlphaTestBatch[i]->PreRender();
				m_skinanimAlphaTestBatch[i]->Render();
				m_skinanimAlphaTestBatch[i]->PostRender();
			}
			ISGPRenderBatch::m_SkinAnimShaderLightmapTexSetted = false;
		}
		break;
	case ISGPRenderBatch::eBatchSkinAnimAlpha:
		{
			ISGPRenderBatch::m_SkinAnimShaderLightmapTexSetted = false;
			for( int i= 0; i<m_skinanimAlphaBatch.size(); i++ )
			{
				m_skinanimAlphaBatch[i]->PreRender();
				m_skinanimAlphaBatch[i]->Render();
				m_skinanimAlphaBatch[i]->PostRender();
			}
			ISGPRenderBatch::m_SkinAnimShaderLightmapTexSetted = false;
		}
		break;
	case ISGPRenderBatch::eBatchLine:
		{
			for( int i= 0; i<m_lineBatch.size(); i++ )
			{
				m_lineBatch[i]->PreRender();
				m_lineBatch[i]->Render();
				m_lineBatch[i]->PostRender();
			}
		}
		break;
	default:
		break;
	}
}

void COpenGLMaterialRenderer::QueueRenderBatch()
{
	COpenGLMaterialRenderer::Sorter CompareRenderBatch;

	if( m_opaqueBatch.size() > 1 )
		m_opaqueBatch.sort(CompareRenderBatch);
	if( m_opaqueLightMapBatch.size() > 1 )
		m_opaqueLightMapBatch.sort(CompareRenderBatch);
	if( m_transparentBatch.size() > 1 )
		m_transparentBatch.sort(CompareRenderBatch);
	if( m_transparentLightMapBatch.size() > 1 )
		m_transparentLightMapBatch.sort(CompareRenderBatch);
	if( m_particlePSBatch.size() > 1 )
		m_particlePSBatch.sort(CompareRenderBatch);
	if( m_particleLineBatch.size() > 1 )
		m_particleLineBatch.sort(CompareRenderBatch);
	if( m_particleQuadBatch.size() > 1 )
		m_particleQuadBatch.sort(CompareRenderBatch);
	if( m_skinanimBatch.size() > 1 )
		m_skinanimBatch.sort(CompareRenderBatch);
	if( m_skinanimAlphaBatch.size() > 1 )
		m_skinanimAlphaBatch.sort(CompareRenderBatch);

	if( m_alphaTestBatch.size() > 1 )
		m_alphaTestBatch.sort(CompareRenderBatch);
	if( m_alphaTestLightMapBatch.size() > 1 )
		m_alphaTestLightMapBatch.sort(CompareRenderBatch);
	if( m_skinanimAlphaTestBatch.size() > 1 )
		m_skinanimAlphaTestBatch.sort(CompareRenderBatch);

}

void COpenGLMaterialRenderer::BeforeDrawRenderBatch()
{
	m_OpaqueRenderBatchPoolUsed = 0;
	m_TransparentRenderBatchPoolUsed = 0;
	m_ParticlePSRenderBatchPoolUsed = 0;
	m_ParticleLineRenderBatchPoolUsed = 0;
	m_ParticleQuadRenderBatchPoolUsed = 0;
	m_LineRenderBatchPoolUsed = 0;
	m_SkinAnimRenderBatchPoolUsed = 0;

	m_opaqueBatch.clearQuick();
	m_opaqueLightMapBatch.clearQuick();
	m_transparentBatch.clearQuick();
	m_transparentLightMapBatch.clearQuick();
	m_particlePSBatch.clearQuick();
	m_particleLineBatch.clearQuick();
	m_particleQuadBatch.clearQuick();
	m_lineBatch.clearQuick();
	m_skinanimBatch.clearQuick();
	m_skinanimAlphaBatch.clearQuick();

	m_alphaTestBatch.clearQuick();
	m_alphaTestLightMapBatch.clearQuick();
	m_skinanimAlphaTestBatch.clearQuick();
}

void COpenGLMaterialRenderer::AfterDrawRenderBatch()
{
	m_OpaqueRenderBatchPoolUsed = 0;
	m_TransparentRenderBatchPoolUsed = 0;
	m_ParticlePSRenderBatchPoolUsed = 0;
	m_ParticleLineRenderBatchPoolUsed = 0;
	m_ParticleQuadRenderBatchPoolUsed = 0;
	m_LineRenderBatchPoolUsed = 0;
	m_SkinAnimRenderBatchPoolUsed = 0;

	m_opaqueBatch.clearQuick();
	m_opaqueLightMapBatch.clearQuick();
	m_transparentBatch.clearQuick();
	m_transparentLightMapBatch.clearQuick();
	m_particlePSBatch.clearQuick();
	m_particleLineBatch.clearQuick();
	m_particleQuadBatch.clearQuick();
	m_lineBatch.clearQuick();
	m_skinanimBatch.clearQuick();
	m_skinanimAlphaBatch.clearQuick();

	m_alphaTestBatch.clearQuick();
	m_alphaTestLightMapBatch.clearQuick();
	m_skinanimAlphaTestBatch.clearQuick();
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_Opaque()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();

	// Normal opaqe RenderBatch
	if( (m_opaqueBatch.size() > 0) || (m_opaqueLightMapBatch.size() > 0) )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &OpaqueMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_opaque_base);
		PushMaterial( OpaqueMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchOpaque );
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_Transparent()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();

	// Normal transparent RenderBatch
	if( (m_transparentBatch.size() > 0) || (m_transparentLightMapBatch.size() > 0) )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &TransparentMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_transparent);
		PushMaterial( TransparentMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchTransparent );
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_DebugLine()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();
	//ISGPMaterialSystem::MaterialList &Mod_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetModifierList();

	// Debug 3D line RenderBatch with depth bias
	if( m_lineBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &LineMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_line);
		//const ISGPMaterialSystem::SGPMaterialInfo &ZBiasModifier_info = Mod_List.getReference(ISGPMaterialSystem::eModifier_depthbias);
		
		//PushMaterial( ZBiasModifier_info.m_material, ZBiasModifier_info.m_material->GetMergeMode() );
		PushMaterial( LineMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchLine );
		PopMaterial();
		//PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_NoDepthline()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();
	ISGPMaterialSystem::MaterialList &Mod_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetModifierList();

	// Debug 3D line RenderBatch without depth test and write
	if( m_lineBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &LineMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_line);
		const ISGPMaterialSystem::SGPMaterialInfo &NoDepthModifier_info = Mod_List.getReference(ISGPMaterialSystem::eModifier_nodepth);
		
		PushMaterial( NoDepthModifier_info.m_material, NoDepthModifier_info.m_material->GetMergeMode() );
		PushMaterial( LineMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchLine );
		PopMaterial();
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_Particle()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();

	// particle point sprites RenderBatch
	if( m_particlePSBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &ParticleMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_particleaddalpha);
		PushMaterial( ParticleMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchParticlePointSprites );
		PopMaterial();
	}

	// particle Line RenderBatch
	if( m_particleLineBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &ParticleMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_particleaddalpha_line);
		PushMaterial( ParticleMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchParticleLine );
		PopMaterial();
	}

	// particle Quad RenderBatch
	if( m_particleQuadBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &ParticleMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_particleaddalpha);
		PushMaterial( ParticleMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchParticleQuad );
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_SkinAnim()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();

	// Skin Anim RenderBatch
	if( m_skinanimBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &SkinAnimMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_opaque_base);
		PushMaterial( SkinAnimMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchSkinAnim );
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::DoDrawRenderBatch_SkinAnimAlpha()
{
	ISGPMaterialSystem::MaterialList &Mat_List = GetOpenGLRenderDevice()->GetMaterialSystem()->GetMaterialList();

	// Skin Anim AVMesh RenderBatch
	if( m_skinanimAlphaBatch.size() > 0 )
	{
		const ISGPMaterialSystem::SGPMaterialInfo &SkinAnimAlphaMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_transparent);
		PushMaterial( SkinAnimAlphaMaterial_info.m_material, MM_Add );
		Draw( ISGPRenderBatch::eBatchSkinAnimAlpha );
		PopMaterial();
	}
}

void COpenGLMaterialRenderer::PushOpaqueRenderBatch(const COpaqueRenderBatch& batch)
{
	COpaqueRenderBatch* pNewBatch = (COpaqueRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchOpaque);
	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;

		if( pNewBatch->m_pSB && pNewBatch->m_pSB->MaterialSkin.bLightMap )
		{
			if( pNewBatch->m_pSB->MaterialSkin.bAlphaTest )
				m_alphaTestLightMapBatch.add(pNewBatch);
			else
				m_opaqueLightMapBatch.add(pNewBatch);
		}
		else
		{
			if( (pNewBatch->m_pSB && pNewBatch->m_pSB->MaterialSkin.bAlphaTest) ||
				(pNewBatch->m_pVC && pNewBatch->m_pVC->m_MaterialSkin.bAlphaTest) )
				m_alphaTestBatch.add(pNewBatch);
			else
				m_opaqueBatch.add(pNewBatch);
		}
	}
}



void COpenGLMaterialRenderer::PushTransparentRenderBatch(const CTransparentRenderBatch& batch)
{
	CTransparentRenderBatch *pNewBatch = (CTransparentRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchTransparent);

	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;

		if( pNewBatch->m_pSB && pNewBatch->m_pSB->MaterialSkin.bLightMap )
			m_transparentLightMapBatch.add(pNewBatch);
		else
			m_transparentBatch.add(pNewBatch);
	}
}

void COpenGLMaterialRenderer::PushLineRenderBatch(const CLineRenderBatch& batch)
{
	CLineRenderBatch* pNewBatch = (CLineRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchLine);	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;

		m_lineBatch.add(pNewBatch);
	}
}

void COpenGLMaterialRenderer::PushParticlePSRenderBatch(const CParticlePSRenderBatch& batch)
{
	CParticlePSRenderBatch* pNewBatch = (CParticlePSRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchParticlePointSprites);	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;
		pNewBatch->m_pParticleBuffer = batch.m_pParticleBuffer;

		m_particlePSBatch.add(pNewBatch);
	}
}

void COpenGLMaterialRenderer::PushParticleLineRenderBatch(const CParticleLineRenderBatch& batch)
{
	CParticleLineRenderBatch* pNewBatch = (CParticleLineRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchParticleLine);	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;
		pNewBatch->m_pParticleBuffer = batch.m_pParticleBuffer;

		m_particleLineBatch.add(pNewBatch);
	}
}

void COpenGLMaterialRenderer::PushParticleQuadRenderBatch(const CParticleQuadRenderBatch& batch)
{
	CParticleQuadRenderBatch* pNewBatch = (CParticleQuadRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchParticleQuad);	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;
		pNewBatch->m_pParticleBuffer = batch.m_pParticleBuffer;
		pNewBatch->m_TextureAtlas = batch.m_TextureAtlas;

		m_particleQuadBatch.add(pNewBatch);
	}
}


void COpenGLMaterialRenderer::PushSkinAnimRenderBatch(const CSkinAnimRenderBatch& batch)
{
	CSkinAnimRenderBatch* pNewBatch = (CSkinAnimRenderBatch*)GetOneFreeRenderBatchFromPool(ISGPRenderBatch::eBatchSkinAnim);	
	if( pNewBatch )
	{
		pNewBatch->m_BatchType = batch.m_BatchType;
		pNewBatch->m_BBOXCenter = batch.m_BBOXCenter;
		pNewBatch->m_MatWorld = batch.m_MatWorld;
		pNewBatch->m_BatchConfig = batch.m_BatchConfig;
		pNewBatch->m_pSB = batch.m_pSB;
		pNewBatch->m_pVC = batch.m_pVC;
		pNewBatch->m_TBOID = batch.m_TBOID;

		if( pNewBatch->m_pSB && pNewBatch->m_pSB->MaterialSkin.bAlphaTest )
			m_skinanimAlphaTestBatch.add(pNewBatch);
		else if( pNewBatch->m_pVC && pNewBatch->m_pVC->m_MaterialSkin.bAlphaTest )
			m_skinanimAlphaTestBatch.add(pNewBatch);
		else if( pNewBatch->m_pSB && (pNewBatch->m_pSB->MaterialSkin.bAlpha || (pNewBatch->m_BatchConfig.m_fBatchAlpha < 1.0f)) )
			m_skinanimAlphaBatch.add(pNewBatch);
		else if( pNewBatch->m_pVC && (pNewBatch->m_pVC->m_MaterialSkin.bAlpha || (pNewBatch->m_BatchConfig.m_fBatchAlpha < 1.0f)) )
			m_skinanimAlphaBatch.add(pNewBatch);
		else
			m_skinanimBatch.add(pNewBatch);
	}
}

ISGPRenderBatch* COpenGLMaterialRenderer::GetOneFreeRenderBatchFromPool(ISGPRenderBatch::BatchType BTtype)
{
	ISGPRenderBatch* pBatch = NULL;
	switch(BTtype)
	{
	case ISGPRenderBatch::eBatchOpaque:
		if( m_OpaqueRenderBatchPoolUsed + 1 >= m_OpaqueRenderBatchPool.size() )
		{
			m_OpaqueRenderBatchPool.add( new COpaqueRenderBatch(m_pRenderDevice) );
			pBatch = m_OpaqueRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_OpaqueRenderBatchPool.getUnchecked(m_OpaqueRenderBatchPoolUsed);
		}	
		m_OpaqueRenderBatchPoolUsed++;
		break;
	case ISGPRenderBatch::eBatchTransparent:
		if( m_TransparentRenderBatchPoolUsed + 1 >= m_TransparentRenderBatchPool.size() )
		{
			m_TransparentRenderBatchPool.add( new CTransparentRenderBatch(m_pRenderDevice) );
			pBatch = m_TransparentRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_TransparentRenderBatchPool.getUnchecked(m_TransparentRenderBatchPoolUsed);
		}
		m_TransparentRenderBatchPoolUsed++;
		break;
	case ISGPRenderBatch::eBatchParticlePointSprites:
		if( m_ParticlePSRenderBatchPoolUsed + 1 >= m_ParticlePSRenderBatchPool.size() )
		{
			m_ParticlePSRenderBatchPool.add( new CParticlePSRenderBatch(m_pRenderDevice) );
			pBatch = m_ParticlePSRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_ParticlePSRenderBatchPool.getUnchecked(m_ParticlePSRenderBatchPoolUsed);
		}
		m_ParticlePSRenderBatchPoolUsed++;
		break;
	case ISGPRenderBatch::eBatchParticleLine:
		if( m_ParticleLineRenderBatchPoolUsed + 1 >= m_ParticleLineRenderBatchPool.size() )
		{
			m_ParticleLineRenderBatchPool.add( new CParticleLineRenderBatch(m_pRenderDevice) );
			pBatch = m_ParticleLineRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_ParticleLineRenderBatchPool.getUnchecked(m_ParticleLineRenderBatchPoolUsed);
		}
		m_ParticleLineRenderBatchPoolUsed++;
		break;
	case ISGPRenderBatch::eBatchParticleQuad:
		if( m_ParticleQuadRenderBatchPoolUsed + 1 >= m_ParticleQuadRenderBatchPool.size() )
		{
			m_ParticleQuadRenderBatchPool.add( new CParticleQuadRenderBatch(m_pRenderDevice) );
			pBatch = m_ParticleQuadRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_ParticleQuadRenderBatchPool.getUnchecked(m_ParticleQuadRenderBatchPoolUsed);
		}
		m_ParticleQuadRenderBatchPoolUsed++;
		break;
	case ISGPRenderBatch::eBatchSkinAnim:
		if( m_SkinAnimRenderBatchPoolUsed + 1 >= m_SkinAnimRenderBatchPool.size() )
		{
			m_SkinAnimRenderBatchPool.add( new CSkinAnimRenderBatch(m_pRenderDevice) );
			pBatch = m_SkinAnimRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_SkinAnimRenderBatchPool.getUnchecked(m_SkinAnimRenderBatchPoolUsed);
		}
		m_SkinAnimRenderBatchPoolUsed++;
		break;

	case ISGPRenderBatch::eBatchLine:
		if( m_LineRenderBatchPoolUsed + 1 >= m_LineRenderBatchPool.size() )
		{
			m_LineRenderBatchPool.add( new CLineRenderBatch(m_pRenderDevice) );
			pBatch = m_LineRenderBatchPool.getLast();
		}
		else
		{
			pBatch = m_LineRenderBatchPool.getUnchecked(m_LineRenderBatchPoolUsed);
		}
		m_LineRenderBatchPoolUsed++;
		break;
	default:
		break;
	}
	return pBatch;
}

void COpenGLMaterialRenderer::ReAllocRenderBatchPool()
{
	for( int i=0; i<INIT_LARGE_RENDERBATCH_ARRAY_NUM; i++ )
	{
		m_OpaqueRenderBatchPool.add( new COpaqueRenderBatch(m_pRenderDevice) );
		m_SkinAnimRenderBatchPool.add( new CSkinAnimRenderBatch(m_pRenderDevice) );
	}
	for( int i=0; i<INIT_SMALL_RENDERBATCH_ARRAY_NUM; i++ )
	{
		m_TransparentRenderBatchPool.add( new CTransparentRenderBatch(m_pRenderDevice) );
		m_ParticlePSRenderBatchPool.add( new CParticlePSRenderBatch(m_pRenderDevice) );
		m_ParticleQuadRenderBatchPool.add( new CParticleQuadRenderBatch(m_pRenderDevice) );
		m_LineRenderBatchPool.add( new CLineRenderBatch(m_pRenderDevice) );
	}
	for( int i=0; i<INIT_FEW_RENDERBATCH_ARRAY_NUM; i++ )
	{
		m_ParticleLineRenderBatchPool.add( new CParticleLineRenderBatch(m_pRenderDevice) );
	}
}

//void COpenGLMaterialRenderer::CommitAdditionalMaterial(const struct ISGPMaterialSystem::SGPMaterialInfo& MT, const struct SGPSkin& Skin )
//{
//
//}
