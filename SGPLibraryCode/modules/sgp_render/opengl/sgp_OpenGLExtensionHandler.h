#ifndef __SGP_OPENGLEXTENSION_HEADER__
#define __SGP_OPENGLEXTENSION_HEADER__


#ifndef GL_ARB_shader_objects
/* GL types for program/shader text and shader object handles */
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
#endif

#ifndef GL_VERSION_2_0
/* GL type for program/shader text */
typedef char GLchar;
#endif

class COpenGLExtensionHandler
{
public:
	// constructor
	COpenGLExtensionHandler();
	virtual ~COpenGLExtensionHandler() {}

	// deferred initialization
	void initExtensions(bool stencilBuffer);

	//! queries the features of the driver, returns true if feature is available
	bool queryDriverFeature(SGP_DRIVER_FEATURE feature) const;


	// Some variables for properties
	bool StencilBuffer;
	bool MultiTextureExtension;
	bool TextureCompressionExtension;
	bool OcclusionQuerySupport;

	// Some non-boolean properties
	//! Maxmimum texture layers supported by the fixed pipeline
	uint8 MaxSupportedTextures;
	//! Maxmimum texture layers supported by the engine
	uint8 MaxTextureUnits;
	//! Maximum hardware lights supported
	uint8 MaxLights;
	//! Maximal Anisotropy
	uint8 MaxAnisotropy;
	//! Number of user clipplanes
	uint8 MaxUserClipPlanes;
	//! Number of auxiliary buffers
	uint8 MaxAuxBuffers;
	//! Number of rendertargets available as MRTs
	uint8 MaxMultipleRenderTargets;
	//! Number of Color Attachement Points for Frame buffer object
	uint8 MaxColorAttachementPoints;
	//! Optimal number of indices per meshbuffer
	uint32 MaxIndices;
	//! Maximal texture dimension
	uint32 MaxTextureSize;
	//! Maximal vertices handled by geometry shaders
	uint32 MaxGeometryVerticesOut;
	//! Maximal LOD Bias
	float MaxTextureLODBias;
	//! Minimal and maximal supported thickness for lines without smoothing
	GLfloat DimAliasedLine[2];
	//! Minimal and maximal supported thickness for points without smoothing
	GLfloat DimAliasedPoint[2];
	//! Minimal and maximal supported thickness for lines with smoothing
	GLfloat DimSmoothedLine[2];
	//! Minimal and maximal supported thickness for points with smoothing
	GLfloat DimSmoothedPoint[2];

	//! OpenGL version as Integer: 100*Major+Minor*10, i.e. 2.1 becomes 210
	uint16 Version;
	//! GLSL version as Integer: 100*Major+Minor*10
	uint16 ShaderLanguageVersion;



	// public access to the (loaded) extensions.
	// general functions
	void extGlActiveTexture(GLenum texture);
	void extGlClientActiveTexture(GLenum texture);
	void extGlPointParameterf(GLint loc, GLfloat f);
	void extGlPointParameterfv(GLint loc, const GLfloat *v);
	void extGlStencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
	void extGlStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
	void extGlCompressedTexImage2D(GLenum target, GLint level,
		GLenum internalformat, GLsizei width, GLsizei height,
		GLint border, GLsizei imageSize, const void* data);

	// Geoemtry Instancing
	void extGlDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
	void extGlDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount);
	void extGlDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLvoid * indices, GLsizei primcount, GLint basevertex);
	void extGlVertexAttribDivisor(GLuint index, GLuint divisor);
	
	// shader programming
	void extGlGenPrograms(GLsizei n, GLuint *programs);
	void extGlBindProgram(GLenum target, GLuint program);
	void extGlProgramString(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
	void extGlLoadProgram(GLenum target, GLuint id, GLsizei len, const GLubyte *string);
	void extGlDeletePrograms(GLsizei n, const GLuint *programs);
	void extGlProgramLocalParameter4fv(GLenum, GLuint, const GLfloat *);
	GLhandleARB extGlCreateShaderObject(GLenum shaderType);
	GLuint extGlCreateShader(GLenum shaderType);
	// note: Due to the type confusion between shader_objects and OpenGL 2.0
	// we have to add the ARB extension for proper method definitions in case
	// that handleARB and uint are the same type
	void extGlShaderSourceARB(GLhandleARB shader, GLsizei numOfStrings, const char **strings, const GLint *lenOfStrings);
	void extGlShaderSource(GLuint shader, GLsizei numOfStrings, const char **strings, const GLint *lenOfStrings);
	// note: Due to the type confusion between shader_objects and OpenGL 2.0
	// we have to add the ARB extension for proper method definitions in case
	// that handleARB and uint are the same type
	void extGlCompileShaderARB(GLhandleARB shader);
	void extGlCompileShader(GLuint shader);
	GLhandleARB extGlCreateProgramObject(void);
	GLuint extGlCreateProgram(void);
	void extGlAttachObject(GLhandleARB program, GLhandleARB shader);
	void extGlAttachShader(GLuint program, GLuint shader);
	void extGlLinkProgramARB(GLhandleARB program);
	// note: Due to the type confusion between shader_objects and OpenGL 2.0
	// we have to add the ARB extension for proper method definitions in case
	// that handleARB and uint are the same type
	void extGlLinkProgram(GLuint program);
	void extGlUseProgramObject(GLhandleARB prog);
	void extGlUseProgram(GLuint prog);
	void extGlDeleteObject(GLhandleARB object);
	void extGlDeleteProgram(GLuint object);
	void extGlDeleteShader(GLuint shader);
	void extGlGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
	void extGlGetAttachedObjects(GLhandleARB program, GLsizei maxcount, GLsizei* count, GLhandleARB* shaders);
	void extGlGetInfoLog(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
	void extGlGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
	void extGlGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
	void extGlGetObjectParameteriv(GLhandleARB object, GLenum type, GLint *param);
	void extGlGetShaderiv(GLuint shader, GLenum type, GLint *param);
	void extGlGetProgramiv(GLuint program, GLenum type, GLint *param);
	GLint extGlGetUniformLocationARB(GLhandleARB program, const char *name);
	GLint extGlGetUniformLocation(GLuint program, const char *name);
	void extGlUniform1fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniform2fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniform3fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniform4fv(GLint loc, GLsizei count, const GLfloat *v);
	//void extGlUniform1bv(GLint loc, GLsizei count, const bool *v);
	//void extGlUniform2bv(GLint loc, GLsizei count, const bool *v);
	//void extGlUniform3bv(GLint loc, GLsizei count, const bool *v);
	//void extGlUniform4bv(GLint loc, GLsizei count, const bool *v);
	void extGlUniform1iv(GLint loc, GLsizei count, const GLint *v);
	void extGlUniform2iv(GLint loc, GLsizei count, const GLint *v);
	void extGlUniform3iv(GLint loc, GLsizei count, const GLint *v);
	void extGlUniform4iv(GLint loc, GLsizei count, const GLint *v);
	void extGlUniformMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void extGlUniformMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void extGlUniformMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void extGlGetActiveUniformARB(GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
	void extGlGetActiveUniform(GLuint program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

	// framebuffer objects
	void extGlBindFramebuffer(GLenum target, GLuint framebuffer);
	void extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
	void extGlGenFramebuffers(GLsizei n, GLuint *framebuffers);
	GLenum extGlCheckFramebufferStatus(GLenum target);
	void extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void extGlBindRenderbuffer(GLenum target, GLuint renderbuffer);
	void extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
	void extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
	void extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void extGlGenerateMipmap(GLenum target);
	void extGlActiveStencilFace(GLenum face);
	void extGlDrawBuffers(GLsizei n, const GLenum *bufs);
	void extGlBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

	// vertex array object
	void extGlGenVertexArray(GLsizei n, GLuint *buffers);
	void extGlDeleteVertexArray(GLsizei n, const GLuint *buffers);
	void extGlBindVertexArray(GLuint buffer);
	void extGlEnableVertexAttribArray(GLuint index);
	void extGlDisableVertexAttribArray(GLuint index);
	void extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *data);

	// vertex buffer object
	void extGlGenBuffers(GLsizei n, GLuint *buffers);
	void extGlBindBuffer(GLenum target, GLuint buffer);
	void extGlBufferData(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
	void extGlDeleteBuffers(GLsizei n, const GLuint *buffers);
	void extGlBufferSubData (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
	void extGlGetBufferSubData (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
	void *extGlMapBuffer (GLenum target, GLenum access);
	void *extGlMapBufferRange (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLbitfield access);
	GLboolean extGlUnmapBuffer (GLenum target);
	GLboolean extGlIsBuffer (GLuint buffer);
	void extGlGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);
	void extGlGetBufferPointerv (GLenum target, GLenum pname, GLvoid **params);
	void extGlProvokingVertex(GLenum mode);
	void extGlColorMaskIndexed(GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
	void extGlEnableIndexed(GLenum target, GLuint index);
	void extGlDisableIndexed(GLenum target, GLuint index);
	void extGlBlendFuncIndexed(GLuint buf, GLenum src, GLenum dst);
	void extGlBlendEquationIndexed(GLuint buf, GLenum mode);
	void extGlProgramParameteri(GLhandleARB program, GLenum pname, GLint value);

	// texture bufferobject
	void extGlTexBuffer(GLenum target, GLenum internalFormat, GLuint buffer);

	// occlusion query
	void extGlGenQueries(GLsizei n, GLuint *ids);
	void extGlDeleteQueries(GLsizei n, const GLuint *ids);
	GLboolean extGlIsQuery(GLuint id);
	void extGlBeginQuery(GLenum target, GLuint id);
	void extGlEndQuery(GLenum target);
	void extGlGetQueryiv(GLenum target, GLenum pname, GLint *params);
	void extGlGetQueryObjectiv(GLuint id, GLenum pname, GLint *params);
	void extGlGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params);

	// generic vsync setting method for several extensions
	void extGlSwapInterval(int interval);

	// blend operations
	void extGlBlendEquation(GLenum mode);

private:
	SGP_DECLARE_NON_COPYABLE (COpenGLExtensionHandler)
};

inline void COpenGLExtensionHandler::extGlActiveTexture(GLenum texture)
{
	if(MultiTextureExtension)
		glActiveTextureARB(texture);
}

inline void COpenGLExtensionHandler::extGlClientActiveTexture(GLenum texture)
{
	if(MultiTextureExtension)
		glClientActiveTextureARB(texture);
}


inline void COpenGLExtensionHandler::extGlDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
#if defined(__GLEE_GL_VERSION_3_1)
	glDrawArraysInstanced(mode, first, count, primcount);
#elif defined(GL_ARB_draw_instanced)
	glDrawArraysInstancedARB(mode, first, count, primcount);
#elif defined(GL_EXT_draw_instanced)
	glDrawArraysInstancedEXT(mode, first, count, primcount) 
#else
	Logger::getCurrentLogger()->writeToLog(String("glDrawArraysInstanced not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount)
{
#if defined(__GLEE_GL_VERSION_3_1)
	glDrawElementsInstanced(mode, count, type, indices, primcount);
#elif defined(GL_ARB_draw_instanced)
	glDrawElementsInstancedARB(mode, count, type, indices, primcount);
#elif defined(GL_EXT_draw_instanced)
	glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDrawElementsInstanced not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLvoid * indices, GLsizei primcount, GLint basevertex)
{
#if defined(GL_ARB_draw_elements_base_vertex)
	glDrawElementsInstancedBaseVertex(mode, count, type, indices, primcount, basevertex);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDrawElementsInstancedBaseVertex not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlVertexAttribDivisor(GLuint index, GLuint divisor)
{
#if defined(__GLEE_GL_VERSION_3_3)
	glVertexAttribDivisor(index, divisor);
#elif defined(GL_ARB_instanced_arrays)
	glVertexAttribDivisorARB(index, divisor);
#else
	Logger::getCurrentLogger()->writeToLog(String("glVertexAttribDivisor not supported"), ELL_ERROR);
#endif
}



inline void COpenGLExtensionHandler::extGlGenPrograms(GLsizei n, GLuint *programs)
{
	if(programs)
		memset(programs, 0, n*sizeof(GLuint));
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glGenProgramsARB(n,programs);
#elif defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)
	glGenProgramsNV(n,programs);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenPrograms not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBindProgram(GLenum target, GLuint program)
{
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glBindProgramARB(target, program);
#elif defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)
	glBindProgramNV(target, program);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBindProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlProgramString(GLenum target, GLenum format, GLsizei len, const GLvoid *string)
{
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glProgramStringARB(target,format,len,string);
#else
	Logger::getCurrentLogger()->writeToLog(String("glProgramString not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlLoadProgram(GLenum target, GLuint id, GLsizei len, const GLubyte *string)
{
#if defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)
	glLoadProgramNV(target,id,len,string);
#else
	Logger::getCurrentLogger()->writeToLog(String("glLoadProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeletePrograms(GLsizei n, const GLuint *programs)
{
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glDeleteProgramsARB(n,programs);
#elif defined(GL_NV_vertex_program) || defined(GL_NV_fragment_program)
	glDeleteProgramsNV(n,programs);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeletePrograms not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlProgramLocalParameter4fv(GLenum n, GLuint i, const GLfloat *f)
{
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glProgramLocalParameter4fvARB(n,i,f);
#else
	Logger::getCurrentLogger()->writeToLog(String("glProgramLocalParameter4fv not supported"), ELL_ERROR);
#endif
}

inline GLhandleARB COpenGLExtensionHandler::extGlCreateShaderObject(GLenum shaderType)
{
#if defined(GL_ARB_shader_objects)
	return glCreateShaderObjectARB(shaderType);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCreateShaderObject not supported"), ELL_ERROR);
#endif
	return 0;
}

inline GLuint COpenGLExtensionHandler::extGlCreateShader(GLenum shaderType)
{
#if defined(GL_VERSION_2_0)
	return glCreateShader(shaderType);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCreateShader not supported"), ELL_ERROR);
	return 0;
#endif
}

inline void COpenGLExtensionHandler::extGlShaderSourceARB(GLhandleARB shader, GLsizei numOfStrings, const char **strings, const GLint *lenOfStrings)
{
#if defined(GL_ARB_shader_objects)
	glShaderSourceARB(shader, numOfStrings, strings, (GLint *)lenOfStrings);
#else
	Logger::getCurrentLogger()->writeToLog(String("glShaderSource not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlShaderSource(GLuint shader, GLsizei numOfStrings, const char **strings, const GLint *lenOfStrings)
{
#if defined(GL_VERSION_2_0)
	glShaderSource(shader, numOfStrings, strings, (GLint *)lenOfStrings);
#else
	Logger::getCurrentLogger()->writeToLog(String("glShaderSource not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlCompileShaderARB(GLhandleARB shader)
{
#if defined(GL_ARB_shader_objects)
	glCompileShaderARB(shader);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCompileShader not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlCompileShader(GLuint shader)
{
#if defined(GL_VERSION_2_0)
	glCompileShader(shader);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCompileShader not supported"), ELL_ERROR);
#endif
}

inline GLhandleARB COpenGLExtensionHandler::extGlCreateProgramObject(void)
{
#if defined(GL_ARB_shader_objects)
	return glCreateProgramObjectARB();
#else
	Logger::getCurrentLogger()->writeToLog(String("glCreateProgramObject not supported"), ELL_ERROR);
#endif
	return 0;
}

inline GLuint COpenGLExtensionHandler::extGlCreateProgram(void)
{
#if defined(GL_VERSION_2_0)
	return glCreateProgram();
#else
	Logger::getCurrentLogger()->writeToLog(String("glCreateProgram not supported"), ELL_ERROR);
	return 0;
#endif
}

inline void COpenGLExtensionHandler::extGlAttachObject(GLhandleARB program, GLhandleARB shader)
{
#if defined(GL_ARB_shader_objects)
	glAttachObjectARB(program, shader);
#else
	Logger::getCurrentLogger()->writeToLog(String("glAttachObject not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlAttachShader(GLuint program, GLuint shader)
{
#if defined(GL_VERSION_2_0)
	glAttachShader(program, shader);
#else
	Logger::getCurrentLogger()->writeToLog(String("glAttachShader not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlLinkProgramARB(GLhandleARB program)
{
#if defined(GL_ARB_shader_objects)
	glLinkProgramARB(program);
#else
	Logger::getCurrentLogger()->writeToLog(String("glLinkProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlLinkProgram(GLuint program)
{
#if defined(GL_VERSION_2_0)
	glLinkProgram(program);
#else
	Logger::getCurrentLogger()->writeToLog(String("glLinkProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUseProgramObject(GLhandleARB prog)
{
#if defined(GL_ARB_shader_objects)
	glUseProgramObjectARB(prog);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUseProgramObject not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUseProgram(GLuint prog)
{
#if defined(GL_VERSION_2_0)
	glUseProgram(prog);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUseProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteObject(GLhandleARB object)
{
#if defined(GL_ARB_shader_objects)
	glDeleteObjectARB(object);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteObject not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteProgram(GLuint object)
{
#if defined(GL_VERSION_2_0)
	glDeleteProgram(object);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteProgram not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteShader(GLuint shader)
{
#if defined(GL_VERSION_2_0)
	glDeleteShader(shader);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteShader not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetAttachedObjects(GLhandleARB program, GLsizei maxcount, GLsizei* count, GLhandleARB* shaders)
{
	if (count)
		*count=0;
#if defined(GL_ARB_shader_objects)
	glGetAttachedObjectsARB(program, maxcount, count, shaders);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetAttachedObjects not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	if (count)
		*count=0;
#if defined(GL_VERSION_2_0)
	glGetAttachedShaders(program, maxcount, count, shaders);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetAttachedShaders not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetInfoLog(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog)
{
	if (length)
		*length=0;
#if defined(GL_ARB_shader_objects)
	glGetInfoLogARB(object, maxLength, length, infoLog);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetInfoLog not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
	if(length)
		*length = 0;

#if defined(GL_VERSION_2_0)
	glGetShaderInfoLog(shader, maxLength, length, infoLog);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetShaderInfoLog not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
	if (length)
		*length=0;

#if defined(GL_VERSION_2_0)
	glGetProgramInfoLog(program, maxLength, length, infoLog);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetProgramInfoLog not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetObjectParameteriv(GLhandleARB object, GLenum type, GLint *param)
{
#if defined(GL_ARB_shader_objects)
	glGetObjectParameterivARB(object, type, param);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetObjectParameteriv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetShaderiv(GLuint shader, GLenum type, GLint *param)
{
#if defined(GL_VERSION_2_0)
	glGetShaderiv(shader, type, param);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetShaderiv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetProgramiv(GLuint program, GLenum type, GLint *param)
{
#if defined(GL_VERSION_2_0)
	glGetProgramiv(program, type, param);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetProgramiv not supported"), ELL_ERROR);
#endif
}

inline GLint COpenGLExtensionHandler::extGlGetUniformLocationARB(GLhandleARB program, const char *name)
{
#if defined(GL_ARB_shader_objects)
	return glGetUniformLocationARB(program, name);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetUniformLocation not supported"), ELL_ERROR);
#endif
	return 0;
}

inline GLint COpenGLExtensionHandler::extGlGetUniformLocation(GLuint program, const char *name)
{
#if defined(GL_VERSION_2_0)
	return glGetUniformLocation(program, name);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetUniformLocation not supported"), ELL_ERROR);
	return 0;
#endif
}

inline void COpenGLExtensionHandler::extGlUniform1fv(GLint loc, GLsizei count, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform1fvARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform1fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform2fv(GLint loc, GLsizei count, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform2fvARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform2fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform3fv(GLint loc, GLsizei count, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform3fvARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform3fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform4fv(GLint loc, GLsizei count, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform4fvARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform4fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform1iv(GLint loc, GLsizei count, const GLint *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform1ivARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform1iv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform2iv(GLint loc, GLsizei count, const GLint *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform2ivARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform2iv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform3iv(GLint loc, GLsizei count, const GLint *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform3ivARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform3iv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniform4iv(GLint loc, GLsizei count, const GLint *v)
{
#if defined(GL_ARB_shader_objects)
	glUniform4ivARB(loc, count, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniform4iv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniformMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniformMatrix2fvARB(loc, count, transpose, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniformMatrix2fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniformMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniformMatrix3fvARB(loc, count, transpose, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniformMatrix3fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlUniformMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#if defined(GL_ARB_shader_objects)
	glUniformMatrix4fvARB(loc, count, transpose, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUniformMatrix4fv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetActiveUniformARB(GLhandleARB program,
		GLuint index, GLsizei maxlength, GLsizei *length,
		GLint *size, GLenum *type, GLcharARB *name)
{
	if (length)
		*length = 0;
#if defined(GL_ARB_shader_objects)
	glGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetActiveUniform not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetActiveUniform(GLuint program,
		GLuint index, GLsizei maxlength, GLsizei *length,
		GLint *size, GLenum *type, GLchar *name)
{
	if (length)
		*length = 0;
#if defined(GL_VERSION_2_0)
	glGetActiveUniform(program, index, maxlength, length, size, type, name);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetActiveUniform not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlPointParameterf(GLint loc, GLfloat f)
{
#if defined(GL_ARB_point_parameters)
	glPointParameterfARB(loc, f);
#else
	Logger::getCurrentLogger()->writeToLog(String("glPointParameterf not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlPointParameterfv(GLint loc, const GLfloat *v)
{
#if defined(GL_ARB_point_parameters)
	glPointParameterfvARB(loc, v);
#else
	Logger::getCurrentLogger()->writeToLog(String("glPointParameterfv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlStencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
#if defined(GL_VERSION_2_0)
	glStencilFuncSeparate(frontfunc, backfunc, ref, mask);
#elif defined(GL_ATI_separate_stencil)
	glStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
#else
	Logger::getCurrentLogger()->writeToLog(String("glStencilFuncSeparate not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
#if defined(GL_VERSION_2_0)
	glStencilOpSeparate(face, fail, zfail, zpass);
#elif defined(GL_ATI_separate_stencil)
	glStencilOpSeparateATI(face, fail, zfail, zpass);
#else
	Logger::getCurrentLogger()->writeToLog(String("glStencilOpSeparate not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
		GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#if defined(GL_ARB_texture_compression)
	glCompressedTexImage2DARB(target, level, internalformat, width, height, border, imageSize, data);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCompressedTexImage2D not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBindFramebuffer(GLenum target, GLuint framebuffer)
{
#if defined(GL_ARB_framebuffer_object)
	glBindFramebuffer(target, framebuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindFramebufferEXT(target, framebuffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBindFramebuffer not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
#if defined(GL_ARB_framebuffer_object)
	glDeleteFramebuffers(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteFramebuffersEXT(n, framebuffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteFramebuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
	if (framebuffers)
		memset(framebuffers,0,n*sizeof(GLuint));
#if defined(GL_ARB_framebuffer_object)
	glGenFramebuffers(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenFramebuffersEXT(n, framebuffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenFramebuffers not supported"), ELL_ERROR);
#endif
}

inline GLenum COpenGLExtensionHandler::extGlCheckFramebufferStatus(GLenum target)
{

#if defined(GL_ARB_framebuffer_object)
	return glCheckFramebufferStatus(target);
#elif defined(GL_EXT_framebuffer_object)
	return glCheckFramebufferStatusEXT(target);
#else
	Logger::getCurrentLogger()->writeToLog(String("glCheckFramebufferStatus not supported"), ELL_ERROR);
	return 0;
#endif
}

inline void COpenGLExtensionHandler::extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#if defined(GL_ARB_framebuffer_object)
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#else
	Logger::getCurrentLogger()->writeToLog(String("glFramebufferTexture2D not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#if defined(GL_ARB_framebuffer_object)
	glBindRenderbuffer(target, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glBindRenderbufferEXT(target, renderbuffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBindRenderbuffer not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
#if defined(GL_ARB_framebuffer_object)
	glDeleteRenderbuffers(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glDeleteRenderbuffersEXT(n, renderbuffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteRenderbuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
	if (renderbuffers)
		memset(renderbuffers,0,n*sizeof(GLuint));
#if defined(GL_ARB_framebuffer_object)
	glGenRenderbuffers(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object)
	glGenRenderbuffersEXT(n, renderbuffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenRenderbuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#if defined(GL_ARB_framebuffer_object)
	glRenderbufferStorage(target, internalformat, width, height);
#elif defined(GL_EXT_framebuffer_object)
	glRenderbufferStorageEXT(target, internalformat, width, height);
#else
	Logger::getCurrentLogger()->writeToLog(String("glRenderbufferStorage not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#if defined(GL_ARB_framebuffer_object)
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(GL_EXT_framebuffer_object)
	glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glFramebufferRenderbuffer not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGenerateMipmap(GLenum target)
{
#if defined(GL_ARB_framebuffer_object)
	glGenerateMipmap(target);
#elif defined(GL_EXT_framebuffer_object)
	glGenerateMipmapEXT(target);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenerateMipmap not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlActiveStencilFace(GLenum face)
{
#if defined(GL_EXT_stencil_two_side)
	glActiveStencilFaceEXT(face);
#else
	Logger::getCurrentLogger()->writeToLog(String("glActiveStencilFace not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDrawBuffers(GLsizei n, const GLenum *bufs)
{
#if defined(GL_ARB_draw_buffers)
	glDrawBuffers(n, bufs);
#elif defined(GL_ATI_draw_buffers)
	glDrawBuffersATI(n, bufs);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDrawBuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
#if defined(GL_ARB_framebuffer_object)
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
#elif defined(GL_EXT_framebuffer_object)
	glBlitFramebufferEXT(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBlitFramebuffer not supported"), ELL_ERROR);
#endif
}


inline void COpenGLExtensionHandler::extGlGenVertexArray(GLsizei n, GLuint *buffers)
{
	if (buffers)
		memset(buffers,0,n*sizeof(GLuint));
#if defined(GL_ARB_vertex_array_object)
	glGenVertexArrays(n, buffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenVertexArrays not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteVertexArray(GLsizei n, const GLuint *buffers)
{
#if defined(GL_ARB_vertex_array_object)
	glDeleteVertexArrays(n, buffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteVertexArrays not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBindVertexArray(GLuint buffer)
{
#if defined(GL_ARB_vertex_array_object)
	glBindVertexArray(buffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBindVertexArray not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlEnableVertexAttribArray(GLuint index)
{
#if defined(GL_ARB_vertex_array_object)
	glEnableVertexAttribArray(index);
#else
	Logger::getCurrentLogger()->writeToLog(String("glEnableVertexAttribArray not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDisableVertexAttribArray(GLuint index)
{
#if defined(GL_ARB_vertex_array_object)
	glDisableVertexAttribArray(index);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDisableVertexAttribArray not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *data)
{
#if defined(GL_ARB_vertex_array_object)
	glVertexAttribPointer(index, size, type, normalized, stride, data);
#else
	Logger::getCurrentLogger()->writeToLog(String("glVertexAttribPointer not supported"), ELL_ERROR);
#endif
}





inline void COpenGLExtensionHandler::extGlGenBuffers(GLsizei n, GLuint *buffers)
{
	if (buffers)
		memset(buffers,0,n*sizeof(GLuint));
#if defined(GL_ARB_vertex_buffer_object)
	glGenBuffers(n, buffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenBuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBindBuffer(GLenum target, GLuint buffer)
{
#if defined(GL_ARB_vertex_buffer_object)
	glBindBuffer(target, buffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBindBuffer not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBufferData(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage)
{
#if defined(GL_ARB_vertex_buffer_object)
	glBufferData(target, size, data, usage);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBufferData not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteBuffers(GLsizei n, const GLuint *buffers)
{
#if defined(GL_ARB_vertex_buffer_object)
	glDeleteBuffers(n, buffers);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteBuffers not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBufferSubData(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data)
{
#if defined(GL_ARB_vertex_buffer_object)
	glBufferSubData(target, offset, size, data);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBufferSubData not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetBufferSubData(GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data)
{
#if defined(GL_ARB_vertex_buffer_object)
	glGetBufferSubData(target, offset, size, data);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetBufferSubData not supported"), ELL_ERROR);
#endif
}

inline void *COpenGLExtensionHandler::extGlMapBuffer(GLenum target, GLenum access)
{
#if defined(GL_ARB_vertex_buffer_object)
	return glMapBuffer(target, access);
#else
	Logger::getCurrentLogger()->writeToLog(String("glMapBuffer not supported"), ELL_ERROR);
	return 0;
#endif
}

inline void *COpenGLExtensionHandler::extGlMapBufferRange(GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLbitfield access)
{
#if defined(GL_ARB_vertex_buffer_object)
	return glMapBufferRange(target, offset, size, access);
#else
	Logger::getCurrentLogger()->writeToLog(String("glMapBufferRange not supported"), ELL_ERROR);
	return 0;
#endif
}

inline GLboolean COpenGLExtensionHandler::extGlUnmapBuffer(GLenum target)
{
#if defined(GL_ARB_vertex_buffer_object)
	return glUnmapBuffer(target);
#else
	Logger::getCurrentLogger()->writeToLog(String("glUnmapBuffer not supported"), ELL_ERROR);
	return false;
#endif
}

inline GLboolean COpenGLExtensionHandler::extGlIsBuffer(GLuint buffer)
{
#if defined(GL_ARB_vertex_buffer_object)
	return glIsBuffer(buffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteBuffers not supported"), ELL_ERROR);
	return false;
#endif
}

inline void COpenGLExtensionHandler::extGlGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
#if defined(GL_ARB_vertex_buffer_object)
	glGetBufferParameteriv(target, pname, params);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetBufferParameteriv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetBufferPointerv(GLenum target, GLenum pname, GLvoid **params)
{
#if defined(GL_ARB_vertex_buffer_object)
	glGetBufferPointerv(target, pname, params);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetBufferPointerv not supported"), ELL_ERROR);
#endif
}


inline void COpenGLExtensionHandler::extGlProvokingVertex(GLenum mode)
{
#if defined(GL_ARB_provoking_vertex)
	glProvokingVertex(mode);
#elif defined(GL_EXT_provoking_vertex)
	glProvokingVertexEXT(mode);
#else
	Logger::getCurrentLogger()->writeToLog(String("glProvokingVertex not supported"), ELL_ERROR);
#endif
}


inline void COpenGLExtensionHandler::extGlColorMaskIndexed(GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
#if defined(GL_EXT_draw_buffers2)
	glColorMaskIndexedEXT(buf, r, g, b, a);
#else
	Logger::getCurrentLogger()->writeToLog(String("glColorMaskIndexed not supported"), ELL_ERROR);
#endif
}


inline void COpenGLExtensionHandler::extGlEnableIndexed(GLenum target, GLuint index)
{
#if defined(GL_EXT_draw_buffers2)
	glEnableIndexedEXT(target, index);
#else
	Logger::getCurrentLogger()->writeToLog(String("glEnableIndexed not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDisableIndexed(GLenum target, GLuint index)
{
#if defined(GL_EXT_draw_buffers2)
	glDisableIndexedEXT(target, index);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDisableIndexed not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBlendFuncIndexed(GLuint buf, GLenum src, GLenum dst)
{
#if defined(GL_ARB_draw_buffers_blend)
	glBlendFunciARB(buf, src, dst);
#elif defined(GL_AMD_draw_buffers_blend)
	glBlendFuncIndexedAMD(buf, src, dst);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBlendFuncIndexed not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlBlendEquationIndexed(GLuint buf, GLenum mode)
{
#if defined(GL_ARB_draw_buffers_blend)
	glBlendEquationiARB(buf, mode);
#elif defined(GL_AMD_draw_buffers_blend)
	glBlendEquationIndexedAMD(buf, mode);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBlendEquationIndexed not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlProgramParameteri(GLhandleARB program, GLenum pname, GLint value)
{
#if defined(GL_ARB_geometry_shader4)
	glProgramParameteriARB(program, pname, value);
#elif defined(GL_EXT_geometry_shader4)
	glProgramParameteriEXT((long GLuint)program, pname, value);
#elif defined(GL_NV_geometry_program4) || defined(GL_NV_geometry_shader4)
	glProgramParameteriNV(program, pname, value);
#else
	Logger::getCurrentLogger()->writeToLog(String("glProgramParameteri not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlTexBuffer(GLenum target, GLenum internalFormat, GLuint buffer)
{
#if defined( GL_ARB_texture_buffer_object )
	glTexBufferARB(target, internalFormat, buffer);
#elif defined( GL_EXT_texture_buffer_object )
	glTexBufferEXT(target, internalFormat, buffer);
#else
	Logger::getCurrentLogger()->writeToLog(String("glTexBuffer not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGenQueries(GLsizei n, GLuint *ids)
{
#if defined(GL_ARB_occlusion_query)
	glGenQueriesARB(n, ids);
#elif defined(GL_NV_occlusion_query)
	glGenOcclusionQueriesNV(n, ids);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGenQueries not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlDeleteQueries(GLsizei n, const GLuint *ids)
{
#if defined(GL_ARB_occlusion_query)
	glDeleteQueriesARB(n, ids);
#elif defined(GL_NV_occlusion_query)
	glDeleteOcclusionQueriesNV(n, ids);
#else
	Logger::getCurrentLogger()->writeToLog(String("glDeleteQueries not supported"), ELL_ERROR);
#endif
}

inline GLboolean COpenGLExtensionHandler::extGlIsQuery(GLuint id)
{
#if defined(GL_ARB_occlusion_query)
	return glIsQueryARB(id);
#elif defined(GL_NV_occlusion_query)
	return glIsOcclusionQueryNV(id);
#else
	return false;
#endif
}

inline void COpenGLExtensionHandler::extGlBeginQuery(GLenum target, GLuint id)
{
#if defined(GL_ARB_occlusion_query)
	glBeginQueryARB(target, id);
#elif defined(GL_NV_occlusion_query)
	glBeginOcclusionQueryNV(id);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBeginQuery not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlEndQuery(GLenum target)
{
#if defined(GL_ARB_occlusion_query)
	glEndQueryARB(target);
#elif defined(GL_NV_occlusion_query)
	glEndOcclusionQueryNV();
#else
	Logger::getCurrentLogger()->writeToLog(String("glEndQuery not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
#if defined(GL_ARB_occlusion_query)
	glGetQueryivARB(target, pname, params);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetQueryivARB not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetQueryObjectiv(GLuint id, GLenum pname, GLint *params)
{
#if defined(GL_ARB_occlusion_query)
	glGetQueryObjectivARB(id, pname, params);
#elif defined(GL_NV_occlusion_query)
	glGetOcclusionQueryivNV(id, pname, params);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetQueryObjectiv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
#if defined(GL_ARB_occlusion_query)
	glGetQueryObjectuivARB(id, pname, params);
#elif defined(GL_NV_occlusion_query)
	glGetOcclusionQueryuivNV(id, pname, params);
#else
	Logger::getCurrentLogger()->writeToLog(String("glGetQueryObjectuiv not supported"), ELL_ERROR);
#endif
}

inline void COpenGLExtensionHandler::extGlSwapInterval(int interval)
{
	// we have wglext, so try to use that
#if SGP_WINDOWS
#ifdef WGL_EXT_swap_control
		wglSwapIntervalEXT(interval);
#endif
#endif

#ifdef SGP_LINUX	
#ifdef GLX_SGI_swap_control
	// does not work with interval==0
	if (interval)
		glXSwapIntervalSGI(interval);
#elif defined(GLX_EXT_swap_control)
	Display *dpy = glXGetCurrentDisplay();
	GLXDrawable drawable = glXGetCurrentDrawable();
	glXSwapIntervalEXT(dpy, drawable, interval);
#endif
#endif
}

inline void COpenGLExtensionHandler::extGlBlendEquation(GLenum mode)
{
#if defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_logic_op)
	glBlendEquationEXT(mode);
#elif defined(GL_VERSION_1_2)
	glBlendEquation(mode);
#else
	Logger::getCurrentLogger()->writeToLog(String("glBlendEquation not supported"), ELL_ERROR);
#endif
}




#endif		// __SGP_OPENGLEXTENSION_HEADER__

