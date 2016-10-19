

} // (juce namespace)

@interface sgpGLView   : UIView
{
}
+ (Class) layerClass;
@end

@implementation sgpGLView
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}
@end

namespace sgp
{



//==============================================================================
bool OpenGLHelpers::isContextActive()
{
    return [EAGLContext currentContext] != nil;
}
