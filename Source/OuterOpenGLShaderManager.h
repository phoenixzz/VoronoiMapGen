#ifndef __OUTER_OPENGLSHADERMANAGER_HEADER__
#define __OUTER_OPENGLSHADERMANAGER_HEADER__

#include "OuterOpenGLShaderTypes.h"

class COuterOpenGLShaderManager
{
public:
	COuterOpenGLShaderManager(COpenGLRenderDevice* pdevice, Logger* logger);	
	virtual ~COuterOpenGLShaderManager();

	virtual void loadAllShaders(void);
	virtual void unloadAllShaders(void);

	COpenGLSLShaderProgram* GetGLSLShaderProgram(int index);

private:
	void loadSingleShader( OUTER_SHADER_TYPE shadertype, const char* VSString, const char* PSString );

private:
	static const int					INIT_OUTERSHADERPROGRAMSIZE = 64;
	COpenGLRenderDevice*				m_pRenderDevice;
	Logger*								m_pLogger;

	OwnedArray<COpenGLSLShader>			m_GLSLShaderArray;
	OwnedArray<COpenGLSLShaderProgram>	m_GLSLProgramArray;
};


#endif		// __OUTER_OPENGLSHADERMANAGER_HEADER__