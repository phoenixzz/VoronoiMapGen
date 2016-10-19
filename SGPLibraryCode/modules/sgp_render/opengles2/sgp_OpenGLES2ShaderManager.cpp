


COpenGLES2ShaderManager::COpenGLES2ShaderManager(COpenGLES2RenderDevice* pdevice, Logger* logger)
	: m_pRenderDevice(pdevice), m_pLogger(logger)
{
	m_GLSLShaderArray.ensureStorageAllocated(INIT_SHADERPROGRAMSIZE*2);
	m_GLSLProgramArray.ensureStorageAllocated(INIT_SHADERPROGRAMSIZE);

	loadAllShaders();
}

COpenGLES2ShaderManager::~COpenGLES2ShaderManager()
{
	unloadAllShaders();
}

void COpenGLES2ShaderManager::loadAllShaders()
{
	// !!!!!!!!!VERY IMPORTENT!!!!!!!!!
	// This order must match SGP_SHADER_TYPE and sBuiltInShaderTypeNames
#include "GLSLES2/glsl_vertexcolor.h"
	loadSingleShader(SGPST_VERTEXCOLOR, Shader_vertexcolor_VS_String, Shader_vertexcolor_PS_String, Shader_vertexcolor_Attribute_String, 2);
#include "GLSLES2/glsl_texture.h"
	loadSingleShader(SGPST_TEXTURE, Shader_texture_VS_String, Shader_texture_PS_String, Shader_texture_Attribute_String, 2);
#include "GLSLES2/glsl_vertexcolor_texture.h"	
	loadSingleShader(SGPST_VERTEXCOLOR_TEXTURE, Shader_vertexcolor_texture_VS_String, Shader_vertexcolor_texture_PS_String, Shader_vertexcolor_texture_Attribute_String, 3);
#include "GLSLES2/glsl_lightmap.h"
	loadSingleShader(SGPST_LIGHTMAP, Shader_lightmap_VS_String, Shader_lightmap_PS_String, Shader_lightmap_Attribute_String, 3);
#include "GLSLES2/glsl_vertexcolor_lightmap.h"
	loadSingleShader(SGPST_VERTEXCOLOR_LIGHTMAP, Shader_vertexcolor_lightmap_VS_String, Shader_vertexcolor_lightmap_PS_String, Shader_vertexcolor_lightmap_Attribute_String, 4);
#include "GLSLES2/glsl_ui.h"
	loadSingleShader(SGPST_UI, Shader_ui_VS_String, Shader_ui_PS_String, Shader_ui_Attribute_String, 3);
#include "GLSLES2/glsl_skeltonanim.h"
	loadSingleShader(SGPST_SKELETONANIM, Shader_anim_VS_String, Shader_anim_PS_String, Shader_anim_Attribute_String, 5);
#include "GLSLES2/glsl_particle_ps.h"
	loadSingleShader(SGPST_PARTICLE_POINTSPRITES, Shader_particlePointSprites_VS_String, Shader_particlePointSprites_PS_String, Shader_particlePointSprites_Attribute_String, 2);
#include "GLSLES2/glsl_particle_line.h"
	loadSingleShader(SGPST_PARTICLE_LINE, Shader_particleLine_VS_String, Shader_particleLine_PS_String, Shader_particleLine_Attribute_String, 2);
#include "GLSLES2/glsl_particle_quad.h"
	loadSingleShader(SGPST_PARTICLE_QUAD_TEXATLAS, Shader_particleQuadTexAtlas_VS_String, Shader_particleQuadTexAtlas_PS_String, Shader_particleQuadTexAtlas_Attribute_String, 4);
/*
	const char* quadVS_ShaderString_NOTEX[3] = { Shader_particleQuad_Version_String, Shader_particleQuad_DefineNoTex, Shader_particleQuad_VS_String };
	GLint quadVS_StringLength_NOTEX[3] = { 14, 20, strlen(Shader_particleQuad_VS_String) };
	//quadVS_StringLength_NOTEX[0] = strlen(Shader_particleQuad_Version_String);
	//quadVS_StringLength_NOTEX[1] = strlen(Shader_particleQuad_DefineNoTex);
	//quadVS_StringLength_NOTEX[2] = strlen(Shader_particleQuad_VS_String);
	const char* quadPS_ShaderString_NOTEX[3] = { Shader_particleQuad_Version_String, Shader_particleQuad_DefineNoTex, Shader_particleQuad_PS_String };
	GLint quadPS_StringLength_NOTEX[3] = { 14, 20, strlen(Shader_particleQuad_PS_String) };
	//quadPS_StringLength_NOTEX[0] = strlen(Shader_particleQuad_Version_String);
	//quadPS_StringLength_NOTEX[1] = strlen(Shader_particleQuad_DefineNoTex);
	//quadPS_StringLength_NOTEX[2] = strlen(Shader_particleQuad_PS_String);
	loadMultiShader(SGPST_PARTICLE_QUAD_NOTEX, 
		3, quadVS_ShaderString_NOTEX, quadVS_StringLength_NOTEX,
		3, quadPS_ShaderString_NOTEX, quadPS_StringLength_NOTEX);
*/

#include "GLSLES2/glsl_terrain_veryhigh.h"
	loadSingleShader(SGPST_TERRAIN_VERYHIGH, Shader_terrain_VeryHigh_VS_String, Shader_terrain_VeryHigh_PS_String, Shader_terrain_VeryHigh_Attribute_String, 4);
#include "GLSLES2/glsl_terrain_lod0.h"
	loadSingleShader(SGPST_TERRAIN_LOD0, Shader_terrain_LOD0_VS_String, Shader_terrain_LOD0_PS_String, Shader_terrain_LOD0_Attribute_String, 4);
#include "GLSLES2/glsl_terrain_lod1.h"
	loadSingleShader(SGPST_TERRAIN_LOD1, Shader_terrain_LOD1_VS_String, Shader_terrain_LOD1_PS_String, Shader_terrain_LOD1_Attribute_String, 4);
#include "GLSLES2/glsl_terrain_lodblend.h"
	loadSingleShader(SGPST_TERRAIN_LODBLEND, Shader_terrain_LODBlend_VS_String, Shader_terrain_LODBlend_PS_String, Shader_terrain_LODBlend_Attribute_String, 4);
#include "GLSLES2/glsl_hoffmanskydome.h"
	loadSingleShader(SGPST_SKYDOMESCATTERING, Shader_hoffmanskydome_VS_String, Shader_hoffmanskydome_PS_String, Shader_hoffmanskydome_Attribute_String, 1);
#include "GLSLES2/glsl_water_refraction.h"
	loadSingleShader(SGPST_WATER_REFRACTION, Shader_waterRefraction_VS_String, Shader_waterRefraction_PS_String, Shader_waterRefraction_Attribute_String, 1);
#include "GLSLES2/glsl_water_surface.h"
	loadSingleShader(SGPST_WATER_RENDER, Shader_waterRender_VS_String, Shader_waterRender_PS_String, Shader_waterRender_Attribute_String, 1);
#include "GLSLES2/glsl_grass.h"
	loadSingleShader(SGPST_GRASS_INSTANCING, Shader_grassRender_VS_String, Shader_grassRender_PS_String, Shader_grassRender_Attribute_String, 4);
}

COpenGLSLES2ShaderProgram* COpenGLES2ShaderManager::GetGLSLShaderProgram(int index)
{
	jassert(index < m_GLSLProgramArray.size());
	return m_GLSLProgramArray.getUnchecked(index);
}

void COpenGLES2ShaderManager::unloadAllShaders()
{
	for( int i=0; i<m_GLSLProgramArray.size(); ++i )
	{
		if( m_GLSLProgramArray[i] )
			m_GLSLProgramArray[i]->deleteProgram();
	}

	for( int j=0; j<m_GLSLShaderArray.size(); ++j )
	{
		if( m_GLSLShaderArray[j] )
			m_GLSLShaderArray[j]->deleteGLSLShader();
	}
}

void COpenGLES2ShaderManager::loadSingleShader(SGP_SHADER_TYPE shadertype, const char* VSString, const char* PSString, const char *AttributeNameString[], int AttributeNameNum )
{
	COpenGLSLES2Shader *pShaderVS = NULL;
	COpenGLSLES2Shader *pShaderPS = NULL;
	COpenGLSLES2ShaderProgram *pShaderProgram = NULL;

	m_pLogger->writeToLog(String("Load and Compile GLSL ES 2.0 Shader ") + String(sBuiltInShaderTypeNames[shadertype]), ELL_INFORMATION);

	pShaderVS = new COpenGLSLES2Shader(m_pRenderDevice);
	pShaderVS->loadGLSLShader( VSString, GL_VERTEX_SHADER );
	pShaderPS = new COpenGLSLES2Shader(m_pRenderDevice);
	pShaderPS->loadGLSLShader( PSString, GL_FRAGMENT_SHADER );


	pShaderProgram = new COpenGLSLES2ShaderProgram(m_pRenderDevice);


	pShaderProgram->createProgram();
	pShaderProgram->addShaderToProgram( pShaderVS);
	pShaderProgram->addShaderToProgram( pShaderPS );

	for( int idx=0; idx<AttributeNameNum; idx++ )
		pShaderProgram->bindAttribLocationToProgram(idx, AttributeNameString[idx]);

	pShaderProgram->linkProgram();


	m_GLSLShaderArray.add( pShaderVS );
	m_GLSLShaderArray.add( pShaderPS );
	m_GLSLProgramArray.add( pShaderProgram );
}

void COpenGLES2ShaderManager::loadMultiShader( SGP_SHADER_TYPE shadertype, int VSNum, const char** VSString, const int* VSStrLength, int PSNum, const char** PSString, const int* PSStrLength, const char *AttributeNameString[], int AttributeNameNum )
{
	COpenGLSLES2Shader *pShaderVS = NULL;
	COpenGLSLES2Shader *pShaderPS = NULL;
	COpenGLSLES2ShaderProgram *pShaderProgram = NULL;

	m_pLogger->writeToLog(String("Load and Compile GLSL ES 2.0 Shader ") + String(sBuiltInShaderTypeNames[shadertype]), ELL_INFORMATION);

	pShaderVS = new COpenGLSLES2Shader(m_pRenderDevice);
	pShaderVS->loadGLSLShader( VSNum, VSString, VSStrLength, GL_VERTEX_SHADER );
	pShaderPS = new COpenGLSLES2Shader(m_pRenderDevice);
	pShaderPS->loadGLSLShader( PSNum, PSString, PSStrLength, GL_FRAGMENT_SHADER );


	pShaderProgram = new COpenGLSLES2ShaderProgram(m_pRenderDevice);


	pShaderProgram->createProgram();
	pShaderProgram->addShaderToProgram( pShaderVS);
	pShaderProgram->addShaderToProgram( pShaderPS );

	for( int idx=0; idx<AttributeNameNum; idx++ )
		pShaderProgram->bindAttribLocationToProgram(idx, AttributeNameString[idx]);

	pShaderProgram->linkProgram();


	m_GLSLShaderArray.add( pShaderVS );
	m_GLSLShaderArray.add( pShaderPS );
	m_GLSLProgramArray.add( pShaderProgram );

}