
#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD) \
 METHOD (layout,        "layout",        "(IIII)V") \
 METHOD (requestRender, "requestRender", "()V") \

DECLARE_JNI_CLASS (OpenGLView, SGP_ANDROID_ACTIVITY_CLASSPATH "$OpenGLView");
#undef JNI_CLASS_MEMBERS

extern jobject createOpenGLView (ComponentPeer*);


//==============================================================================
bool OpenGLHelpers::isContextActive()
{
    //return OpenGLContext::NativeContext::getActiveContext() != nullptr;
	return true;
}

//==============================================================================
#define GL_VIEW_CLASS_NAME    SGP_JOIN_MACRO (SGP_ANDROID_ACTIVITY_CLASSNAME, _00024OpenGLView)

SGP_JNI_CALLBACK (GL_VIEW_CLASS_NAME, contextCreated, void, (JNIEnv* env, jobject view))
{
    threadLocalJNIEnvHolder.getOrAttach();

    if (OpenGLContext::NativeContext* const context = OpenGLContext::NativeContext::findContextFor (env, view))
        context->contextCreatedCallback();
    else
        jassertfalse;
}

SGP_JNI_CALLBACK (GL_VIEW_CLASS_NAME, contextChangedSize, void, (JNIEnv* env, jobject view))
{
    if (OpenGLContext::NativeContext* const context = OpenGLContext::NativeContext::findContextFor (env, view))
        context->contextChangedSize();
}

SGP_JNI_CALLBACK (GL_VIEW_CLASS_NAME, render, void, (JNIEnv* env, jobject view))
{
    if (OpenGLContext::NativeContext* const context = OpenGLContext::NativeContext::findContextFor (env, view))
        context->renderCallback();
}
