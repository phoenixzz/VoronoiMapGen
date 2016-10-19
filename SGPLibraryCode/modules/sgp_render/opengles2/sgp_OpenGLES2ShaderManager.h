#ifndef __SGP_OPENGLES2SHADERMANAGER_HEADER__
#define __SGP_OPENGLES2SHADERMANAGER_HEADER__



class COpenGLES2ShaderManager : public ISGPShaderManager
{
public:
	COpenGLES2ShaderManager(COpenGLES2RenderDevice* pdevice, Logger* logger);	
	virtual ~COpenGLES2ShaderManager();

	virtual void onDeviceLost(void) {}
	virtual void onDeviceReset(void) {}
	virtual void preCacheShaders(void) {}
	virtual void loadAllShaders(void);
	virtual void unloadAllShaders(void);


	COpenGLSLES2ShaderProgram* GetGLSLShaderProgram(int index);

private:
	void loadSingleShader( SGP_SHADER_TYPE shadertype, const char* VSString, const char* PSString, const char *AttributeNameString[], int AttributeNameNum );
	void loadMultiShader( SGP_SHADER_TYPE shadertype, int VSNum, const char** VSString, const int* VSStrLength, int PSNum, const char** PSString, const int* PSStrLength, const char *AttributeNameString[], int AttributeNameNum );

private:
	static const int						INIT_SHADERPROGRAMSIZE = 128;
	COpenGLES2RenderDevice*					m_pRenderDevice;
	Logger*									m_pLogger;

	OwnedArray<COpenGLSLES2Shader>			m_GLSLShaderArray;
	OwnedArray<COpenGLSLES2ShaderProgram>	m_GLSLProgramArray;
};



#endif		// __SGP_OPENGLES2SHADERMANAGER_HEADER__