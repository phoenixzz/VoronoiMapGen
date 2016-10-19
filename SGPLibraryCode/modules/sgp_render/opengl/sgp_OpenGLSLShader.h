#ifndef __SGP_OPENGLSLSHADER_HEADER__
#define __SGP_OPENGLSLSHADER_HEADER__

// Class For OpenGL shader loading and compiling.

class COpenGLSLShader
{
public:

	//! Constructor
	COpenGLSLShader(COpenGLRenderDevice* pRenderDevice);
	virtual ~COpenGLSLShader() {}

	bool loadGLSLShader(const char* shaderStr, GLenum shaderType);
	bool loadGLSLShader(GLsizei strnum, const char** shaderStr, const GLint* StrLength, GLenum shaderType);
	void deleteGLSLShader();

	inline bool IsLoaded() { return m_bLoaded; }
	inline GLuint getShaderID() { return m_ShaderID; }

private:
	COpenGLRenderDevice*	m_pRenderDevice;	// Render Device
	GLuint					m_ShaderID;			// OPENGL ID of shader
	GLenum					m_ShaderType;		// GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool					m_bLoaded;			// Whether shader was loaded and compiled
};



class COpenGLSLShaderProgram
{
public:
	COpenGLSLShaderProgram(COpenGLRenderDevice* pRenderDevice);
	virtual ~COpenGLSLShaderProgram() {}

	void createProgram();
	void deleteProgram();

	bool addShaderToProgram(COpenGLSLShader* pShader);
	bool linkProgram();

	void useProgram();

	inline GLuint GetProgramID() { return m_ProgramID; }

	// Setting vectors
	void setShaderUniform(const char* sName, const Vector2D& vVector);
	void setShaderUniform(const char* sName, Vector2D* vVectors, int32 iCount = 1);
	void setShaderUniform(const char* sName, const Vector3D& vVector);
	void setShaderUniform(const char* sName, Vector3D* vVectors, int32 iCount = 1);
	void setShaderUniform(const char* sName, const Vector4D& vVector);
	void setShaderUniform(const char* sName, Vector4D* vVectors, int32 iCount = 1);

	// Setting floats
	void setShaderUniform(const char* sName, float* fValues, int32 iCount = 1);
	void setShaderUniform(const char* sName, const float fValue);


	// Setting 4x4 matrices
	void setShaderUniform(const char* sName, Matrix4x4* mMatrices, int32 iCount = 1);
	void setShaderUniform(const char* sName, const Matrix4x4& mMatrix);

	// Setting integers
	void setShaderUniform(const char* sName, int32* iValues, int32 iCount = 1);
	void setShaderUniform(const char* sName, const int32 iValue);

public:
	static GLuint			m_CurrentProgramID; // current used Program ID

private:
	COpenGLRenderDevice*	m_pRenderDevice;	// Render Device
	GLuint					m_ProgramID;		// OpenGL ID of program
	bool					m_bLinked;			// Whether program was linked and is ready to use
};



#endif		// __SGP_OPENGLSLSHADER_HEADER__