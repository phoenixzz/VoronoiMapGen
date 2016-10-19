const char *Shader_particlePointSprites_Attribute_String[] = { "inPosition", "inColor" };

char Shader_particlePointSprites_VS_String[] = 
	"																								\n"\
	"attribute highp vec3 inPosition;																\n"\
	"attribute lowp vec4 inColor;																	\n"\
	"																								\n"\
	"uniform highp mat4 modelViewProjMatrix;														\n"\
	"uniform highp mat4 modelViewMatrix;															\n"\
	"uniform highp vec4 Thickness;																	\n"\
	"																								\n"\
	"varying lowp vec4 vVertexColorPass;															\n"\
	"																								\n"\
	" void main()																					\n"\
	" {																								\n"\
	"	vVertexColorPass = inColor;																	\n"\
	"	highp vec4 VertexinView = modelViewMatrix * vec4(inPosition, 1.0);							\n"\
	"	highp float dist = length(VertexinView);													\n"\
	"	highp float attn = inversesqrt(Thickness.x + Thickness.y*dist + Thickness.z*dist*dist);		\n"\

    "	// Custom size adjustment											\n"\
    "	gl_PointSize = attn * Thickness.w;									\n"\

	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	" }																		\n"\
	"";


// Add-alpha pixel shader. To be used in conjunction
// with the blend factors {One, InverseSourceAlpha}
char Shader_particlePointSprites_PS_String[] = 
	"																						\n"\
	"varying lowp vec4 vVertexColorPass;													\n"\

	"uniform sampler2D gSampler0;															\n"\
	"uniform lowp float mode;																\n"\

	"void main()																			\n"\
	"{																						\n"\
	"	lowp vec4 TexColor = texture2D(gSampler0, gl_PointCoord);							\n"\

	"	// Pre-multiply the texture alpha. For alpha-blended particles,						\n"\
	"	// this achieves the same effect as a SourceAlpha blend factor						\n"\
	"	lowp vec3 preMultipliedColour = TexColor.rgb * TexColor.a * vVertexColorPass.a;		\n"\
	"	lowp vec3 colourOut = vVertexColorPass.rgb * preMultipliedColour;					\n"\
	"																						\n"\
	"	// The vertex alpha controls whether the particle is								\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,						\n"\
	"	// or an intermediate value for a mix of both										\n"\
	"	lowp float alphaOut = vVertexColorPass.a * TexColor.a * mode;						\n"\

	"	gl_FragColor = vec4( colourOut, alphaOut );											\n"\
	"}																						\n"\

	"";
