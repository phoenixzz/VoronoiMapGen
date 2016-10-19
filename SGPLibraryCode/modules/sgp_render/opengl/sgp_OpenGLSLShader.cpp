

COpenGLSLShader::COpenGLSLShader(COpenGLRenderDevice* pRenderDevice) 
	: m_pRenderDevice(pRenderDevice), m_bLoaded(false), m_ShaderID(0)
{
}

bool COpenGLSLShader::loadGLSLShader(const char* shaderStr, GLenum shaderType)
{
	m_ShaderID = m_pRenderDevice->extGlCreateShader(shaderType);

	m_pRenderDevice->extGlShaderSource(m_ShaderID, 1, &shaderStr, NULL);
	m_pRenderDevice->extGlCompileShader(m_ShaderID);

#if defined( DEBUG ) || defined( _DEBUG )
	GLint iCompilationStatus = 0;
	m_pRenderDevice->extGlGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL shader failed to compile"), ELL_ERROR);
		
		// check error message and log it
		GLint maxLength = 0;
		GLint length;

		m_pRenderDevice->extGlGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if( maxLength )
		{
			GLchar *infoLog = new GLchar[maxLength];
			m_pRenderDevice->extGlGetShaderInfoLog(m_ShaderID, maxLength, &length, infoLog);
			Logger::getCurrentLogger()->writeToLog(String(reinterpret_cast<const char*>(infoLog)), ELL_ERROR);
			delete [] infoLog;
			infoLog = NULL;
		}

		return false;
	}
#endif

	m_ShaderType = shaderType;
	m_bLoaded = true;

	return true;
}

bool COpenGLSLShader::loadGLSLShader( GLsizei strnum, const char** shaderStr, const GLint* StrLength, GLenum shaderType )
{
	m_ShaderID = m_pRenderDevice->extGlCreateShader(shaderType);

	m_pRenderDevice->extGlShaderSource(m_ShaderID, strnum, shaderStr, StrLength);
	m_pRenderDevice->extGlCompileShader(m_ShaderID);

#if defined( DEBUG ) || defined( _DEBUG )	
	GLint iCompilationStatus = 0;
	m_pRenderDevice->extGlGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL shader failed to compile"), ELL_ERROR);
		
		// check error message and log it
		GLint maxLength = 0;
		GLint length;

		m_pRenderDevice->extGlGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if( maxLength )
		{
			GLchar *infoLog = new GLchar[maxLength];
			m_pRenderDevice->extGlGetShaderInfoLog(m_ShaderID, maxLength, &length, infoLog);
			Logger::getCurrentLogger()->writeToLog(String(reinterpret_cast<const char*>(infoLog)), ELL_ERROR);
			delete [] infoLog;
			infoLog = NULL;
		}

		return false;
	}
#endif
	m_ShaderType = shaderType;
	m_bLoaded = true;

	return true;
}

void COpenGLSLShader::deleteGLSLShader()
{
	if( !IsLoaded() )
		return;
	m_bLoaded = false;
	m_pRenderDevice->extGlDeleteShader(m_ShaderID);
}





GLuint COpenGLSLShaderProgram::m_CurrentProgramID = 0;

COpenGLSLShaderProgram::COpenGLSLShaderProgram(COpenGLRenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_bLinked(false)
{
}

void COpenGLSLShaderProgram::createProgram()
{
	m_ProgramID = m_pRenderDevice->extGlCreateProgram();
}

void COpenGLSLShaderProgram::deleteProgram()
{
	if(!m_bLinked)
		return;
	m_bLinked = false;
	m_pRenderDevice->extGlDeleteProgram(m_ProgramID);
}

bool COpenGLSLShaderProgram::addShaderToProgram(COpenGLSLShader* pShader)
{
	if(!pShader || !pShader->IsLoaded())
		return false;

	m_pRenderDevice->extGlAttachShader(m_ProgramID, pShader->getShaderID());

	return true;
}

bool COpenGLSLShaderProgram::linkProgram()
{
	m_pRenderDevice->extGlLinkProgram(m_ProgramID);

#if defined( DEBUG ) || defined( _DEBUG )
	GLint iLinkStatus = 0;
	m_pRenderDevice->extGlGetProgramiv(m_ProgramID, GL_LINK_STATUS, &iLinkStatus);
	m_bLinked = (iLinkStatus == GL_TRUE);

	if( !iLinkStatus )
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL shader program failed to link"), ELL_ERROR);
	
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		m_pRenderDevice->extGlGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength)
		{
			GLchar *infoLog = new GLchar[maxLength];
			m_pRenderDevice->extGlGetProgramInfoLog(m_ProgramID, maxLength, &length, infoLog);
			Logger::getCurrentLogger()->writeToLog(String(reinterpret_cast<const char*>(infoLog)), ELL_ERROR);
			delete [] infoLog;
			infoLog = NULL;
		}
		return m_bLinked;	
	}
#else
	m_bLinked = true;
#endif

	return m_bLinked;
}

void COpenGLSLShaderProgram::useProgram()
{
	if( m_bLinked && (m_CurrentProgramID != m_ProgramID) )
	{
		m_pRenderDevice->extGlUseProgram(m_ProgramID);
		m_CurrentProgramID = m_ProgramID;
	}
}

// Setting vectors
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const Vector2D& vVector)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform2fv(Location, 1, &(vVector.x));
}
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, Vector2D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform2fv(Location, iCount, (GLfloat*)(vVectors));
}

void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const Vector3D& vVector)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform3fv(Location, 1, &(vVector.x));
}
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, Vector3D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform3fv(Location, iCount, (GLfloat*)(vVectors));
}

void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const Vector4D& vVector)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform4fv(Location, 1, &(vVector.x));
}
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, Vector4D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform4fv(Location, iCount, (GLfloat*)(vVectors));
}

// Setting floats
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, float* fValues, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform1fv(Location, iCount, fValues);
}

void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const float fValue)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform1fv(Location, 1, &fValue);
}

// Setting 4x4 matrices
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, Matrix4x4* mMatrices, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniformMatrix4fv(Location, iCount, false, (GLfloat*)mMatrices);
}

void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const Matrix4x4& mMatrix)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniformMatrix4fv(Location, 1, false, (GLfloat*)(&mMatrix._11));
}

// Setting integers
void COpenGLSLShaderProgram::setShaderUniform(const char* sName, int32* iValues, int32 iCount)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform1iv(Location, iCount, iValues);
}

void COpenGLSLShaderProgram::setShaderUniform(const char* sName, const int32 iValue)
{
	GLint Location = 0;
	Location = m_pRenderDevice->extGlGetUniformLocation(m_ProgramID, sName);
	m_pRenderDevice->extGlUniform1iv(Location, 1, &iValue);
}