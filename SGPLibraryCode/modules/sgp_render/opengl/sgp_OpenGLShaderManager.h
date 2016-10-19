#ifndef __SGP_OPENGLSHADERMANAGER_HEADER__
#define __SGP_OPENGLSHADERMANAGER_HEADER__



class COpenGLShaderManager : public ISGPShaderManager
{
public:
	COpenGLShaderManager(COpenGLRenderDevice* pdevice, Logger* logger);	
	virtual ~COpenGLShaderManager();

	virtual void onDeviceLost(void) {}
	virtual void onDeviceReset(void) {}
	virtual void preCacheShaders(void) {}
	virtual void loadAllShaders(void);
	virtual void unloadAllShaders(void);


	COpenGLSLShaderProgram* GetGLSLShaderProgram(int index);

private:
	void loadSingleShader( SGP_SHADER_TYPE shadertype, const char* VSString, const char* PSString );
	void loadMultiShader( SGP_SHADER_TYPE shadertype, int VSNum, const char** VSString, const int* VSStrLength, int PSNum, const char** PSString, const int* PSStrLength );

private:
	static const int					INIT_SHADERPROGRAMSIZE = 128;
	COpenGLRenderDevice*				m_pRenderDevice;
	Logger*								m_pLogger;

	OwnedArray<COpenGLSLShader>			m_GLSLShaderArray;
	OwnedArray<COpenGLSLShaderProgram>	m_GLSLProgramArray;
};



#endif		// __SGP_OPENGLSHADERMANAGER_HEADER__