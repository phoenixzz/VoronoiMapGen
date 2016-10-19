

ISGPParticleRenderer::ISGPParticleRenderer(ISGPRenderDevice* device) :
	m_renderDevice(device),
	m_currentBuffer(NULL)
{
	m_material.VertexType = SGPVT_UPOS_VERTEXCOLOR;	
}



//void ISGPParticleRenderer::setBlending(SPARK::BlendingMode blendMode)
//{
	//switch(blendMode)
	//{
	//case SPARK::BLENDING_NONE:
	//	blendSrcFunc = SGPBF_ONE;
	//	blendDestFunc = SGPBF_ZERO;
	//	alphaSource = SGPAS_NONE;
	//	break;

	//case SPARK::BLENDING_ADD:
	//	blendSrcFunc = SGPBF_SRC_ALPHA;
	//	blendDestFunc = SGPBF_ONE;
	//	alphaSource = SGPAS_VERTEX_COLOR | SGPAS_TEXTURE;
	//	break;

	//case SPARK::BLENDING_ALPHA:
	//	blendSrcFunc = SGPBF_SRC_ALPHA;
	//	blendDestFunc = SGPBF_ONE_MINUS_SRC_ALPHA;
	//	alphaSource = SGPAS_VERTEX_COLOR | SGPAS_TEXTURE;
	//	break;
	//}
	//updateMaterialBlendingMode();
//}

//void ISGPParticleRenderer::enableRenderingHint(SPARK::RenderingHint renderingHint, bool enable)
//{
	//switch(renderingHint)
	//{
	//case SPARK::DEPTH_TEST:
	//	m_material.ZTestEnable = enable;
	//	break;

	//case SPARK::DEPTH_WRITE:
	//	m_material.ZWriteEnable = enable;
	//	break;
	//}
//}

//bool ISGPParticleRenderer::isRenderingHintEnabled(SPARK::RenderingHint renderingHint) const
//{
	//switch(renderingHint)
	//{
	//case SPARK::DEPTH_TEST:
	//	return m_material.ZTestEnable;

	//case SPARK::DEPTH_WRITE:
	//	return m_material.ZWriteEnable;

	//case SPARK::ALPHA_TEST:
	//	return true; // always enabled
	//}

	//return false;
//}