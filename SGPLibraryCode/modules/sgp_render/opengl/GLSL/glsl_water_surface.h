

char Shader_waterRender_VS_String [] =
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec4 inPosition;								\n"\
/*
	"layout (location = 1) in vec3 inNormal;								\n"\
	"layout (location = 2) in vec3 inTangent;								\n"\
	"layout (location = 3) in vec3 inBinormal;								\n"\
	"layout (location = 4) in vec2 inCoord0;								\n"\
	"layout (location = 5) in vec2 inCoord1;								\n"\
*/
	"uniform mat4 worldViewProjMatrix;										\n"\
	"uniform vec3 vSunDir;					// from 0,0,0 to sun position	\n"\
	"uniform vec4 vCameraPos;												\n"\
	"uniform vec4 vWaveParams;												\n"\
	"uniform vec2 vWaveDir;													\n"\

	"out vec3 vOutWave0;													\n"\
	"out vec2 vOutWave1;													\n"\
	"out vec2 vOutWave2;													\n"\
	"out vec2 vOutWave3;													\n"\
	"out vec3 vOutEye;														\n"\
    "out vec4 vOutScreenPos;												\n"\
    "out vec3 vOutLight;													\n"\
																			
	" void main()															\n"\
	" {																		\n"\
	"	mat3 objToTanMatrix;												\n"\
	"	objToTanMatrix[0] = vec3(1.0, 0.0, 0.0);							\n"\
	"	objToTanMatrix[1] = vec3(0.0, 0.0, 1.0);							\n"\
	"	objToTanMatrix[2] = vec3(0.0, 1.0, 0.0);							\n"\

	"	// vWaveParams.y is water height									\n"\
	"	// vWaveParams.z is scale											\n"\
	"	// vOutWave0.z is water's height - terrain's height					\n"\
	"	vOutWave0.z = (vWaveParams.y - inPosition.y) / vWaveParams.z;		\n"\
	"	vec3 inputPos = vec3(inPosition.x, vWaveParams.y, inPosition.z);	\n"\

	" 	gl_Position = worldViewProjMatrix * vec4(inputPos, 1.0);			\n"\

	"	vec2 fTranslation = vec2(mod(vWaveParams.x, 100) * -vWaveDir);		\n"\
    "	vec2 vTexCoords = inputPos.xz * vWaveParams.w;						\n"\

	"	// Scale texture coords to get mix of low/high frequency details	\n"\
    "	vOutWave0.xy = vTexCoords.xy + fTranslation*2;						\n"\
    "	vOutWave1.xy = vTexCoords.xy*2 + fTranslation*4;					\n"\
    "	vOutWave2.xy = vTexCoords.xy*4 + fTranslation*2;					\n"\
    "	vOutWave3.xy = vTexCoords.xy*8 + fTranslation;						\n"\
																			
	"	// perspective corrected projection									\n"\
    "	vec4 vHPos = gl_Position;											\n"\
	"	vOutScreenPos.xy = (vHPos.xy + vHPos.w)*0.5;						\n"\
    "	vOutScreenPos.zw = vec2(1.0, vHPos.w);								\n"\
																			
	"	vec3 EyeVec = vCameraPos.xyz - inputPos.xyz;						\n"\
    "	vOutEye = normalize(objToTanMatrix * EyeVec);						\n"\
   																			
    "	//vec3 LightVec = vLightPos.xyz - IN.Pos.xyz;						\n"\
	"	vOutLight = normalize(objToTanMatrix * vSunDir);					\n"\

	" }																		\n"\
	"";



char Shader_waterRender_PS_String [] =
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) out vec4 outputColor;							\n"\

	"in vec3 vOutWave0;														\n"\
	"in vec2 vOutWave1;														\n"\
	"in vec2 vOutWave2;														\n"\
	"in vec2 vOutWave3;														\n"\
	"in	vec3 vOutEye;														\n"\
	"in	vec4 vOutScreenPos;													\n"\
	"in	vec3 vOutLight;														\n"\


	"uniform sampler2D gSampler0;		// Bumpmap wave Texture				\n"\
	"uniform sampler2D gSampler1;		// Refraction Texture				\n"\
	"uniform sampler2D gSampler2;		// Reflection Texture				\n"\

	"uniform vec4 vWaterBumpFresnel;	// Bump and Fresnel Params			\n"\
	"uniform vec4 vWaterColor;			// Water color(w is sun spec power)	\n"\
	"uniform vec2 vWaterDepthBlend;		// water depth Params				\n"\
	"uniform vec4 vSunColor;			// sun color for specular color		\n"\


	"// This assumes NdotL comes clamped																\n"\
	"float Fresnel(float NdotL, float fresnelBias, float fresnelPow)									\n"\
	"{																									\n"\
	"	float facing = (1.0 - NdotL);																	\n"\
	"	return max(fresnelBias + (1.0-fresnelBias) * pow(facing, fresnelPow), 0.0);						\n"\
	"}																									\n"\

	"void main()																						\n"\
	"{																									\n"\
	"	// Get bump layers																				\n"\
	"	vec3 vBumpTexA = texture2D(gSampler0, vOutWave0.xy).xyz;										\n"\
	"	vec3 vBumpTexB = texture2D(gSampler0, vOutWave1.xy).xyz;										\n"\
	"	vec3 vBumpTexC = texture2D(gSampler0, vOutWave2.xy).xyz;										\n"\
	"	vec3 vBumpTexD = texture2D(gSampler0, vOutWave3.xy).xyz;										\n"\
																										
	"	// Average bump layers																			\n"\
	"	vec3 vBumpTex = normalize(2.0*(vBumpTexA + vBumpTexB + vBumpTexC + vBumpTexD)-4.0);				\n"\
																										
	"	// Apply individual bump scale for refraction and reflection									\n"\
	"	vec3 vRefrBump = vBumpTex.xyz * vec3(vWaterBumpFresnel.x, vWaterBumpFresnel.x, 1.0);			\n"\
	"	vec3 vReflBump = vBumpTex.xyz * vec3(vWaterBumpFresnel.y, vWaterBumpFresnel.y, 0.25); 			\n"\
																										
	"	// Compute projected coordinates																\n"\
	"	vec2 vProj = vOutScreenPos.xy / vOutScreenPos.w;												\n"\
	"	vec4 vReflection = texture2D(gSampler2, (vProj.xy + vReflBump.xy)*0.5);							\n"\
																										
	"	// Mask occluders from refraction map															\n"\
	"	vec4 vRefrA = texture2D(gSampler1, vProj.xy + vRefrBump.xy);									\n"\
	"	vec4 vRefrB = texture2D(gSampler1, vProj.xy);													\n"\
																										
	"	// Mask occluders from refraction map															\n"\
	"	vec4 vRefraction = mix( vRefrA, vRefrB, vRefrA.w );												\n"\
																								
	"	// Compute specular highlight																	\n"\
	"	vec3 NormalT = normalize( vReflBump );															\n"\
	"	vec3 specular = vec3(0,0,0);																	\n"\
	"	if( vWaterColor.w > 0 )																			\n"\
	"	{																								\n"\
	"		vec3 vRef = reflect(vOutEye, NormalT);														\n"\
	"		specular = pow(max(0, dot(vRef, -vOutLight)), vWaterColor.w) * vSunColor.xyz;				\n"\
	"	}																								\n"\
	
	"	// Compute Fresnel term																			\n"\
	"	float NdotL = max(dot(vOutEye, NormalT), 0.0);													\n"\
	"	float facing = (1.0 - NdotL);																	\n"\
	"	float fresnel = Fresnel(NdotL, vWaterBumpFresnel.z, vWaterBumpFresnel.w);						\n"\

	"	// Use distance to lerp between refraction and deep water color									\n"\
	"	float fDistScale = clamp(vWaterDepthBlend.x/(vOutWave0.z * vWaterDepthBlend.y), 0.0, 1.0);		\n"\
	"	vec3 WaterDeepColor = mix( vWaterColor.xyz, vRefraction.xyz, fDistScale );						\n"\

	"	// Lerp between water color and deep water color												\n"\
	"	vec3 waterColor = mix( WaterDeepColor, vWaterColor.xyz, facing );								\n"\
	"	vec3 cReflect = fresnel * vReflection.xyz;														\n"\

    "	outputColor = vec4(cReflect+waterColor+specular, clamp(vOutWave0.z*vWaterDepthBlend.y,0.0,1.0));\n"\
	"}																									\n"\

	"";