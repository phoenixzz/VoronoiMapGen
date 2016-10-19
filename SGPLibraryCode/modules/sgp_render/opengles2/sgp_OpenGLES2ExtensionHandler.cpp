

COpenGLES2ExtensionHandler::COpenGLES2ExtensionHandler() :
	m_bDiscardFramebuffer(false),
	m_bVertexArrayObject(false),
	m_bDrawBuffers(false),
	m_bOcclusionQuery(false),
	m_bMapBuffer(false),
	m_bMultiDrawArrays(false)
{
	m_Extensions.LoadExtensions();

	m_bDiscardFramebuffer = CPVRTgles2Ext::IsGLExtensionSupported("GL_EXT_discard_framebuffer");
	m_bVertexArrayObject = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_vertex_array_object");
	m_bDrawBuffers = CPVRTgles2Ext::IsGLExtensionSupported("GL_EXT_draw_buffers");
	m_bOcclusionQuery = CPVRTgles2Ext::IsGLExtensionSupported("GL_EXT_occlusion_query_boolean");
	m_bMapBuffer = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_mapbuffer");
	m_bMapBufferRange = CPVRTgles2Ext::IsGLExtensionSupported("GL_EXT_map_buffer_range");
	m_bMultiDrawArrays = CPVRTgles2Ext::IsGLExtensionSupported("GL_EXT_multi_draw_arrays");
}

bool COpenGLES2ExtensionHandler::isDiscardFramebufferSupported()
{
	return ( m_bDiscardFramebuffer && (m_Extensions.glDiscardFramebufferEXT != 0) );
}
