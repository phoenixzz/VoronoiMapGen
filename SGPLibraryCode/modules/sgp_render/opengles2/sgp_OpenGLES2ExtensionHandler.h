#ifndef __SGP_OPENGLES2EXTENSION_HEADER__
#define __SGP_OPENGLES2EXTENSION_HEADER__



class COpenGLES2ExtensionHandler
{
public:
	bool m_bDiscardFramebuffer;		// Whether discard is available or not
	bool m_bVertexArrayObject;		// Whether VAO is available or not
	bool m_bDrawBuffers;			// GL_EXT_draw_buffers
	bool m_bOcclusionQuery;			// GL_EXT_occlusion_query_boolean
	bool m_bMapBuffer;				// GL_OES_mapbuffer
	bool m_bMapBufferRange;			// GL_EXT_map_buffer_range
	bool m_bMultiDrawArrays;		// GL_EXT_multi_draw_arrays

public:
	// constructor
	COpenGLES2ExtensionHandler();
	virtual ~COpenGLES2ExtensionHandler() {}


	// Geoemtry Instancing
	void extGlMultiDrawElements(GLenum mode, GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount);
	void extGlMultiDrawArrays(GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);

	// Discard
	void extGlDiscardFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments);

	// framebuffer objects
	void extGlRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlFramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);

	// GL_EXT_draw_buffers
	void extGlDrawBuffers(GLsizei n, const GLenum *bufs);


	// vertex array object
	void extGlGenVertexArray(GLsizei n, GLuint *arrays);
	void extGlDeleteVertexArray(GLsizei n, const GLuint *arrays);
	void extGlBindVertexArray(GLuint array);
	GLboolean extGlIsVertexArray(GLuint array);


	// vertex buffer object
	void * extGlMapBuffer (GLenum target, GLenum access);
	GLboolean extGlUnmapBuffer (GLenum target);
	void extGlGetBufferPointerv (GLenum target, GLenum pname, GLvoid **params);
	void* extGlMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
	void  extGlFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length);
	
	// occlusion query
	void extGlGenQueries(GLsizei n, GLuint *ids);
	void extGlDeleteQueries(GLsizei n, const GLuint *ids);
	GLboolean extGlIsQuery(GLuint id);
	void extGlBeginQuery(GLenum target, GLuint id);
	void extGlEndQuery(GLenum target);
	void extGlGetQueryiv(GLenum target, GLenum pname, GLint *params);
	void extGlGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params);

public:
	bool isDiscardFramebufferSupported();

private:
	SGP_DECLARE_NON_COPYABLE (COpenGLES2ExtensionHandler)

	CPVRTgles2Ext	m_Extensions;	// GL Extensions Class
};

inline void COpenGLES2ExtensionHandler::extGlMultiDrawElements(GLenum mode, GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount)
{
	if( m_bMultiDrawArrays && (m_Extensions.glMultiDrawElementsEXT != 0) )
		m_Extensions.glMultiDrawElementsEXT(mode, count, type, indices, primcount);
	else
		Logger::getCurrentLogger()->writeToLog(String("glMultiDrawElementsEXT not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlMultiDrawArrays(GLenum mode, GLint *first, GLsizei *count, GLsizei primcount)
{
	if( m_bMultiDrawArrays && (m_Extensions.glMultiDrawArraysEXT != 0) )
		m_Extensions.glMultiDrawArraysEXT(mode, first, count, primcount);
	else
		Logger::getCurrentLogger()->writeToLog(String("glMultiDrawArraysEXT not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlDiscardFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
	if( m_bDiscardFramebuffer && (m_Extensions.glDiscardFramebufferEXT != 0) )
		m_Extensions.glDiscardFramebufferEXT(target, numAttachments, attachments);
	else
		Logger::getCurrentLogger()->writeToLog(String("glDiscardFramebufferEXT not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	if( m_Extensions.glRenderbufferStorageMultisampleEXT != 0 )
		m_Extensions.glRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
	else if( m_Extensions.glRenderbufferStorageMultisampleIMG != 0 )
		m_Extensions.glRenderbufferStorageMultisampleIMG(target, samples, internalformat, width, height);
	else
		Logger::getCurrentLogger()->writeToLog(String("glRenderbufferStorageMultisampleEXT and glRenderbufferStorageMultisampleIMG not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlFramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples)
{
	if( m_Extensions.glFramebufferTexture2DMultisampleEXT != 0 )
		m_Extensions.glFramebufferTexture2DMultisampleEXT(target, attachment, textarget, texture, level, samples);
	else if( m_Extensions.glFramebufferTexture2DMultisampleIMG != 0 )
		m_Extensions.glFramebufferTexture2DMultisampleIMG(target, attachment, textarget, texture, level, samples);
	else
		Logger::getCurrentLogger()->writeToLog(String("glFramebufferTexture2DMultisampleEXT and glFramebufferTexture2DMultisampleIMG not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlDrawBuffers(GLsizei n, const GLenum *bufs)
{
	if( m_bDrawBuffers && m_Extensions.glDrawBuffersEXT != 0 )
		m_Extensions.glDrawBuffersEXT(n, bufs);
	else
		Logger::getCurrentLogger()->writeToLog(String("glDrawBuffers not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlGenVertexArray(GLsizei n, GLuint *arrays)
{
	if(arrays)
		memset(arrays, 0, n*sizeof(GLuint));

#ifdef TARGET_OS_IPHONE
	if( m_bVertexArrayObject )
		glGenVertexArraysOES(n, arrays);
#else
	if( m_bVertexArrayObject && (m_Extensions.glGenVertexArraysOES != 0) )
		m_Extensions.glGenVertexArraysOES(n, arrays);
#endif	
	else
		Logger::getCurrentLogger()->writeToLog(String("glGenVertexArrays not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlDeleteVertexArray(GLsizei n, const GLuint *arrays)
{
#ifdef TARGET_OS_IPHONE
	if( m_bVertexArrayObject )
		glDeleteVertexArraysOES(n, arrays);
#else
	if( m_bVertexArrayObject && m_Extensions.glDeleteVertexArraysOES != 0 )
		m_Extensions.glDeleteVertexArraysOES(n, arrays);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glDeleteVertexArrays not supported"), ELL_ERROR);

}

inline void COpenGLES2ExtensionHandler::extGlBindVertexArray(GLuint array)
{
#ifdef TARGET_OS_IPHONE
	if( m_bVertexArrayObject )
		glBindVertexArrayOES(array);
#else
	if( m_bVertexArrayObject && m_Extensions.glBindVertexArrayOES != 0 )
		m_Extensions.glBindVertexArrayOES(array);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glBindVertexArray not supported"), ELL_ERROR);
}

inline GLboolean COpenGLES2ExtensionHandler::extGlIsVertexArray(GLuint array)
{
#ifdef TARGET_OS_IPHONE
	if( m_bVertexArrayObject )
		return glIsVertexArrayOES(array);
#else
	if( m_bVertexArrayObject && m_Extensions.glIsVertexArrayOES != 0 )
		return m_Extensions.glIsVertexArrayOES(array);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glIsVertexArray not supported"), ELL_ERROR);
	return false;
}


inline void *COpenGLES2ExtensionHandler::extGlMapBuffer(GLenum target, GLenum access)
{
#ifdef TARGET_OS_IPHONE
	if( m_bMapBuffer )
		return glMapBufferOES(target, access);
#else
	if( m_bMapBuffer && m_Extensions.glMapBufferOES != 0 )
		return m_Extensions.glMapBufferOES(target, access);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glMapBuffer not supported"), ELL_ERROR);
	return 0;
}

inline GLboolean COpenGLES2ExtensionHandler::extGlUnmapBuffer(GLenum target)
{
#ifdef TARGET_OS_IPHONE
	if( m_bMapBuffer )
		return glUnmapBufferOES(target);
#else
	if( m_bMapBuffer && m_Extensions.glUnmapBufferOES != 0)
		return m_Extensions.glUnmapBufferOES(target);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glUnmapBuffer not supported"), ELL_ERROR);
	return false;
}

inline void COpenGLES2ExtensionHandler::extGlGetBufferPointerv(GLenum target, GLenum pname, GLvoid **params)
{
#ifdef TARGET_OS_IPHONE
	if( m_bMapBuffer )
		glGetBufferPointervOES(target, pname, params);
#else
	if( m_bMapBuffer && m_Extensions.glGetBufferPointervOES != 0 )
		m_Extensions.glGetBufferPointervOES(target, pname, params);
#endif
	else
		Logger::getCurrentLogger()->writeToLog(String("glGetBufferPointerv not supported"), ELL_ERROR);
}


inline void* COpenGLES2ExtensionHandler::extGlMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	if( m_bMapBufferRange && m_Extensions.glMapBufferRangeEXT != 0 )
		return m_Extensions.glMapBufferRangeEXT(target, offset, length, access);
	else
	{
		Logger::getCurrentLogger()->writeToLog(String("glMapBufferRange not supported"), ELL_ERROR);
		return 0;
	}
}

inline void COpenGLES2ExtensionHandler::extGlFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length)
{
	if( m_bMapBufferRange && m_Extensions.glFlushMappedBufferRangeEXT != 0 )
		m_Extensions.glFlushMappedBufferRangeEXT(target, offset, length);
	else
		Logger::getCurrentLogger()->writeToLog(String("glFlushMappedBufferRange not supported"), ELL_ERROR);
}



inline void COpenGLES2ExtensionHandler::extGlGenQueries(GLsizei n, GLuint *ids)
{
	if( m_bOcclusionQuery && m_Extensions.glGenQueriesEXT != 0 )
		m_Extensions.glGenQueriesEXT(n, ids);
	else
		Logger::getCurrentLogger()->writeToLog(String("glGenQueries not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlDeleteQueries(GLsizei n, const GLuint *ids)
{
	if( m_bOcclusionQuery && m_Extensions.glDeleteQueriesEXT != 0 )
		m_Extensions.glDeleteQueriesEXT(n, ids);
	else
		Logger::getCurrentLogger()->writeToLog(String("glDeleteQueries not supported"), ELL_ERROR);
}

inline GLboolean COpenGLES2ExtensionHandler::extGlIsQuery(GLuint id)
{
	if( m_bOcclusionQuery && m_Extensions.glIsQueryEXT != 0 )
		return m_Extensions.glIsQueryEXT(id);
	else
		return false;
}

inline void COpenGLES2ExtensionHandler::extGlBeginQuery(GLenum target, GLuint id)
{
	if( m_bOcclusionQuery && m_Extensions.glBeginQueryEXT != 0 )
		m_Extensions.glBeginQueryEXT(target, id);
	else
		Logger::getCurrentLogger()->writeToLog(String("glBeginQuery not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlEndQuery(GLenum target)
{
	if( m_bOcclusionQuery && m_Extensions.glEndQueryEXT != 0 )
		m_Extensions.glEndQueryEXT(target);
	else
		Logger::getCurrentLogger()->writeToLog(String("glEndQuery not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
	if( m_bOcclusionQuery && m_Extensions.glGetQueryivEXT != 0 )
		m_Extensions.glGetQueryivEXT(target, pname, params);
	else
		Logger::getCurrentLogger()->writeToLog(String("glGetQueryivARB not supported"), ELL_ERROR);
}

inline void COpenGLES2ExtensionHandler::extGlGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
	if( m_bOcclusionQuery && m_Extensions.glGetQueryObjectuivEXT != 0 )
		m_Extensions.glGetQueryObjectuivEXT(id, pname, params);
	else
		Logger::getCurrentLogger()->writeToLog(String("glGetQueryObjectuiv not supported"), ELL_ERROR);
}


#endif		// __SGP_OPENGLES2EXTENSION_HEADER__

