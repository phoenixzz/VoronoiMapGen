

char Shader_particlePointSprites_VS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec3 inPosition;								\n"\
	"layout (location = 1) in vec4 inColor;									\n"\
	"																		\n"\
	"uniform mat4 modelViewProjMatrix;										\n"\
	"uniform mat4 modelViewMatrix;											\n"\
	"uniform vec4 Thickness;												\n"\
	"																		\n"\
	"out vec4 vVertexColorPass;												\n"\
	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	"	vVertexColorPass = inColor;											\n"\
	"	vec4 VertexinView = modelViewMatrix * vec4(inPosition, 1.0);		\n"\
	"	float dist = length(VertexinView);									\n"\
	"	float attn = inversesqrt(Thickness.x + Thickness.y*dist + Thickness.z*dist*dist);			\n"\

    "	// Custom size adjustment											\n"\
    "	gl_PointSize = attn * Thickness.w;									\n"\

    "	// If they are very small, fade them up								\n"\
    "	//if(gl_PointSize < 4.0)											\n"\
	"	//{																	\n"\
    "	//	vVertexColorPass = smoothstep(0.0, 4.0, gl_PointSize) *			\n"\
	"	//						vVertexColorPass;							\n"\
	"	//}																	\n"\

	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	" }																		\n"\
	"";


// Add-alpha pixel shader. To be used in conjunction
// with the blend factors {One, InverseSourceAlpha}
char Shader_particlePointSprites_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"in vec4 vVertexColorPass;												\n"\
	"out vec4 outputColor;													\n"\

	"uniform sampler2D gSampler0;											\n"\
	"uniform float mode;													\n"\

	"void main()															\n"\
	"{																		\n"\
	"	vec4 TexColor = texture2D(gSampler0, gl_PointCoord);				\n"\

	"	// Pre-multiply the texture alpha. For alpha-blended particles,		\n"\
	"	// this achieves the same effect as a SourceAlpha blend factor		\n"\
	"	vec3 preMultipliedColour = TexColor.rgb * TexColor.a * vVertexColorPass.a;	\n"\
	"	vec3 colourOut = vVertexColorPass.rgb * preMultipliedColour;		\n"\
	"																		\n"\
	"	// The vertex alpha controls whether the particle is				\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,		\n"\
	"	// or an intermediate value for a mix of both						\n"\
	"	float alphaOut = vVertexColorPass.a * TexColor.a * mode;			\n"\

	"	outputColor = vec4( colourOut, alphaOut );							\n"\
	"}																		\n"\

	"";
