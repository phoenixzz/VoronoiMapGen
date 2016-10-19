


//==============================================================================
bool OpenGLHelpers::isContextActive()
{
    ScopedXLock xlock;
    return glXGetCurrentContext() != 0;
}
