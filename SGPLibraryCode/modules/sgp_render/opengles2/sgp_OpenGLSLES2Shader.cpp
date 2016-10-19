

COpenGLSLES2Shader::COpenGLSLES2Shader(COpenGLES2RenderDevice* pRenderDevice) 
	: m_pRenderDevice(pRenderDevice), m_bLoaded(false), m_ShaderID(0)
{
}

bool COpenGLSLES2Shader::loadGLSLShader(const char* shaderStr, GLenum shaderType)
{
	m_ShaderID = glCreateShader(shaderType);

	glShaderSource(m_ShaderID, 1, &shaderStr, NULL);
	glCompileShader(m_ShaderID);

#if defined( DEBUG ) || defined( _DEBUG )	
	GLint iCompilationStatus = 0;
	glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL ES 2.0 shader failed to compile"), ELL_ERROR);
		
		// check error message and log it
		GLint maxLength = 0;
		GLint length;

		glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if( maxLength )
		{
			GLchar *infoLog = new GLchar[maxLength+1];
			glGetShaderInfoLog(m_ShaderID, maxLength, &length, infoLog);
			infoLog[length] = 0;
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

bool COpenGLSLES2Shader::loadGLSLShader( GLsizei strnum, const char** shaderStr, const GLint* StrLength, GLenum shaderType )
{
	m_ShaderID = glCreateShader(shaderType);

	glShaderSource(m_ShaderID, strnum, shaderStr, StrLength);
	glCompileShader(m_ShaderID);

#if defined( DEBUG ) || defined( _DEBUG )	
	GLint iCompilationStatus = 0;
	glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL shader failed to compile"), ELL_ERROR);
		
		// check error message and log it
		GLint maxLength = 0;
		GLint length;

		glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if( maxLength )
		{
			GLchar *infoLog = new GLchar[maxLength+1];
			glGetShaderInfoLog(m_ShaderID, maxLength, &length, infoLog);
			infoLog[length] = 0;
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

void COpenGLSLES2Shader::deleteGLSLShader()
{
	if( !IsLoaded() )
		return;
	m_bLoaded = false;
	glDeleteShader(m_ShaderID);
}





GLuint COpenGLSLES2ShaderProgram::m_CurrentProgramID = 0;

COpenGLSLES2ShaderProgram::COpenGLSLES2ShaderProgram(COpenGLES2RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_bLinked(false)
{
}

void COpenGLSLES2ShaderProgram::createProgram()
{
	m_ProgramID = glCreateProgram();
}

void COpenGLSLES2ShaderProgram::deleteProgram()
{
	if(!m_bLinked)
		return;
	m_bLinked = false;
	glDeleteProgram(m_ProgramID);
}

bool COpenGLSLES2ShaderProgram::addShaderToProgram(COpenGLSLES2Shader* pShader)
{
	if(!pShader || !pShader->IsLoaded())
		return false;

	glAttachShader(m_ProgramID, pShader->getShaderID());

	return true;
}

bool COpenGLSLES2ShaderProgram::bindAttribLocationToProgram(GLuint index, const GLchar *name)
{
	if( !name )
		return false;

	glBindAttribLocation(m_ProgramID, index, name);

	return true;
}

bool COpenGLSLES2ShaderProgram::linkProgram()
{
	glLinkProgram(m_ProgramID);

#if defined( DEBUG ) || defined( _DEBUG )
	GLint iLinkStatus = 0;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &iLinkStatus);
	m_bLinked = (iLinkStatus == GL_TRUE);

	if( !iLinkStatus )
	{
		Logger::getCurrentLogger()->writeToLog(String("GLSL ES 2.0 shader program failed to link"), ELL_ERROR);
	
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength)
		{
			GLchar *infoLog = new GLchar[maxLength+1];
			glGetProgramInfoLog(m_ProgramID, maxLength, &length, infoLog);
			infoLog[length] = 0;
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

void COpenGLSLES2ShaderProgram::useProgram()
{
	if( m_bLinked && (m_CurrentProgramID != m_ProgramID) )
	{
		glUseProgram(m_ProgramID);
		m_CurrentProgramID = m_ProgramID;
	}
}

// Setting vectors
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const Vector2D& vVector)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform2fv(Location, 1, &(vVector.x));
}
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, Vector2D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform2fv(Location, iCount, (GLfloat*)(vVectors));
}

void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const Vector3D& vVector)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform3fv(Location, 1, &(vVector.x));
}
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, Vector3D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform3fv(Location, iCount, (GLfloat*)(vVectors));
}

void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const Vector4D& vVector)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform4fv(Location, 1, &(vVector.x));
}
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, Vector4D* vVectors, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform4fv(Location, iCount, (GLfloat*)(vVectors));
}

// Setting floats
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, float* fValues, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform1fv(Location, iCount, fValues);
}

void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const float fValue)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform1fv(Location, 1, &fValue);
}

// Setting 4x4 matrices
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, Matrix4x4* mMatrices, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniformMatrix4fv(Location, iCount, false, (GLfloat*)mMatrices);
}

void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const Matrix4x4& mMatrix)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniformMatrix4fv(Location, 1, false, (GLfloat*)(&mMatrix._11));
}

// Setting integers
void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, int32* iValues, int32 iCount)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform1iv(Location, iCount, iValues);
}

void COpenGLSLES2ShaderProgram::setShaderUniform(const char* sName, const int32 iValue)
{
	GLint Location = 0;
	Location = glGetUniformLocation(m_ProgramID, sName);
	glUniform1iv(Location, 1, &iValue);
}