char Shader_hoffmanskydome_VS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec3 inPosition;								\n"\
/*
	"layout (location = 1) in vec2 inCoord;									\n"\
*/

	"uniform mat4 modelViewProjMatrix;										\n"\
	"uniform mat4 modelViewMatrix;											\n"\
	"uniform vec3 InvSunDir;												\n"\
	"uniform vec3 BetaRPlusBetaM;											\n"\
																			
	"out vec2 vTexCoordPass;												\n"\
	"out vec3 vInPosition;													\n"\
	"out vec3 vSunInvDirection;												\n"\
	"out vec3 vOutExtinction;												\n"\

	" void main()															\n"\
	" {																		\n"\
	" 	 vec4 position = modelViewProjMatrix * vec4(inPosition, 1.0);		\n"\
	"	 gl_Position = position.xyww;										\n"\
	" 	 vTexCoordPass.xy = inPosition.xz;									\n"\
	"	 vInPosition = inPosition;											\n"\
	
	"	 // Extinction term													\n"\
	"	 vec4 vertex = modelViewMatrix * vec4(inPosition, 1.0);				\n"\
	"	 float distance = length( vertex.xyz ) * 1000.0;  					\n"\
	"	 vOutExtinction = exp( -BetaRPlusBetaM * distance );				\n"\

	"	 vec4 sunInvDirection = modelViewMatrix * vec4(InvSunDir, 1.0);		\n"\
	"	 vSunInvDirection = normalize(sunInvDirection.xyz);					\n"\

	" }																		\n"\
	"";


char Shader_hoffmanskydome_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"in vec2 vTexCoordPass;													\n"\
	"in vec3 vInPosition;													\n"\
	"in vec3 vSunInvDirection;												\n"\
	"in vec3 vOutExtinction;												\n"\

	"layout (location = 0) out vec4 outputColor;							\n"\

	"uniform sampler2D gSampler0;		// Cloud texture					\n"\
	"uniform sampler2D gSampler1;		// Noise texture					\n"\
	"uniform vec4 cloudParam;			// cloud movement param				\n"\

	"uniform mat4 modelViewMatrix;											\n"\
	"uniform vec3 HGg;														\n"\
	"uniform vec4 Multipliers;												\n"\
	"uniform vec3 BetaDashR;												\n"\
	"uniform vec3 BetaDashM;												\n"\
	"uniform vec3 OneOverBetaRPlusBetaM;									\n"\
	"uniform vec4 SunColorAndIntensity;										\n"\

	"void main()															\n"\
	"{																		\n"\
	"	vec2 coludUV = vTexCoordPass;										\n"\
	"	vec4 perturbValue;													\n"\

	"	vec4 vertex = modelViewMatrix * vec4(vInPosition, 1.0);				\n"\
	"	vec3 vViewDirection = normalize(vertex.xyz);						\n"\

	"	float theta = dot( vSunInvDirection, vViewDirection );									\n"\
	"	// Phase1 and Phase2																	\n"\
	"	float phase1 = 1.0 + theta * theta;														\n"\
	"	float phase2 = pow( inversesqrt(HGg.y - HGg.z * theta), 3 ) * HGg.x;					\n"\

	"	// Inscattering term																	\n"\
	"	vec3 betaRay = BetaDashR * phase1;														\n"\
	"	vec3 betaMie = BetaDashM * phase2;														\n"\
	"	vec3 inscatter = (betaRay + betaMie) * OneOverBetaRPlusBetaM * (1.0 - vOutExtinction);	\n"\
																								
	"	// Apply inscattering contribution factors												\n"\
	"	inscatter *= Multipliers.x;																\n"\
	
	"	// Scale with sun color & intensity														\n"\
	"	inscatter *= SunColorAndIntensity.xyz * 100.0;											\n"\

	"	outputColor = vec4(inscatter, 1.0);														\n"\

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