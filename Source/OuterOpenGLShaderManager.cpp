
#include "../SGPLibraryCode/SGPHeader.h"
#include "OuterOpenGLShaderManager.h"

COuterOpenGLShaderManager::COuterOpenGLShaderManager(COpenGLRenderDevice* pdevice, Logger* logger)
	: m_pRenderDevice(pdevice), m_pLogger(logger)
{
	m_GLSLShaderArray.ensureStorageAllocated(INIT_OUTERSHADERPROGRAMSIZE*2);
	m_GLSLProgramArray.ensureStorageAllocated(INIT_OUTERSHADERPROGRAMSIZE);

	loadAllShaders();
}

COuterOpenGLShaderManager::~COuterOpenGLShaderManager()
{
	unloadAllShaders();
}

void COuterOpenGLShaderManager::loadAllShaders()
{
#include "outershaders/FilterNormal.h"
	loadSingleShader(Filters_Normal, Normal_VS_String, Normal_PS_String);

}

COpenGLSLShaderProgram* COuterOpenGLShaderManager::GetGLSLShaderProgram(int index)
{
	jassert(index < m_GLSLProgramArray.size());
	return m_GLSLProgramArray.getUnchecked(index);
}

void COuterOpenGLShaderManager::unloadAllShaders()
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

void COuterOpenGLShaderManager::loadSingleShader( OUTER_SHADER_TYPE shadertype, const char* VSString, const char* PSString )
{
	COpenGLSLShader *pShaderVS = NULL;
	COpenGLSLShader *pShaderPS = NULL;
	COpenGLSLShaderProgram *pShaderProgram = NULL;

	m_pLogger->writeToLog(String("Load and Compile GLSL Shader ") + String(sOuterShaderTypeNames[shadertype]), ELL_INFORMATION);

	pShaderVS = new COpenGLSLShader(m_pRenderDevice);
	pShaderVS->loadGLSLShader( VSString, GL_VERTEX_SHADER );
	pShaderPS = new COpenGLSLShader(m_pRenderDevice);
	pShaderPS->loadGLSLShader( PSString, GL_FRAGMENT_SHADER );


	pShaderProgram = new COpenGLSLShaderProgram(m_pRenderDevice);


	pShaderProgram->createProgram();
	pShaderProgram->addShaderToProgram( pShaderVS);
	pShaderProgram->addShaderToProgram( pShaderPS );

	pShaderProgram->linkProgram();


	m_GLSLShaderArray.add( pShaderVS );
	m_GLSLShaderArray.add( pShaderPS );
	m_GLSLProgramArray.add( pShaderProgram );
}