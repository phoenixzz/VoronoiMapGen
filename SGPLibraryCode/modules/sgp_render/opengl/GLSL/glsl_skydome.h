char Shader_skydome_VS_String[] = 
	"#version 330														\n"\
	"																	\n"\
	"layout (location = 0) in vec3 inPosition;							\n"\
/*
	"layout (location = 1) in vec2 inCoord;								\n"\
*/

	"uniform mat4 modelViewProjMatrix;									\n"\

	"out vec2 vTexCoordPass;											\n"\
	"out vec3 vDomePosition;											\n"\

	" void main()														\n"\
	" {																	\n"\
	" 	 vec4 position = modelViewProjMatrix * vec4(inPosition, 1.0);	\n"\
	"	 gl_Position = position.xyww;									\n"\
	" 	 vTexCoordPass.xy = inPosition.xz;								\n"\
	"	 vDomePosition = inPosition;									\n"\
	" }																	\n"\
	"";


char Shader_skydome_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"in vec2 vTexCoordPass;													\n"\
	"in vec3 vDomePosition;													\n"\

	"layout(location = 0) out vec4 outputColor;								\n"\

	"uniform sampler2D gSampler0;		// Cloud texture					\n"\
	"uniform sampler2D gSampler1;		// Noise texture					\n"\
	"uniform vec4 cloudParam;			// cloud movement param				\n"\
	"uniform vec4 apexColor;												\n"\
    "uniform vec4 centerColor;												\n"\
	"uniform float maxSkyHeight;											\n"\

	"void main()															\n"\
	"{																		\n"\
	"	vec2 coludUV = vTexCoordPass;										\n"\
	"	vec4 perturbValue;													\n"\
	"	float fHeight = 1.0 - (maxSkyHeight-vDomePosition.y)/maxSkyHeight;	\n"\
	"	outputColor = mix(centerColor, apexColor, fHeight);					\n"\

	"	// Translate the texture coordinate sampling location				\n"\
	"	// by the translation value.										\n"\
    "	coludUV.x = coludUV.x + cloudParam.x;								\n"\

    "	// Sample the texture value from the noise texture using			\n"\
	"	// the translated texture coordinates.								\n"\
	"	// Also multiply the perturb value by the perturb scale.			\n"\
    "	perturbValue = texture2D(gSampler1, coludUV) * cloudParam.z;		\n"\

    "	// Add the texture coordinates as well as the translation value		\n"\
	"	// to get the perturbed texture coordinate sampling location.		\n"\
    "	perturbValue.xy = perturbValue.xy + coludUV.xy + cloudParam.xy;		\n"\

	"   // Now sample the color from the cloud texture using				\n"\
	"	// the perturbed sampling coordinates.								\n"\
	"	// Also alpha blend the cloud color with the brightness value.		\n"\
	"	vec4 cloudcolor = texture2D(gSampler0, perturbValue.xy);			\n"\
    "	outputColor.xyz += outputColor.xyz * (1.0-cloudcolor.w*cloudParam.w) + cloudcolor.xyz * cloudParam.w * cloudcolor.w;	\n"\
	"}																		\n"\

	"";