const char *Shader_waterRender_Attribute_String[] = { "inPosition" };


char Shader_waterRender_VS_String [] =
	"																		\n"\
	"attribute highp vec4 inPosition;										\n"\

	"uniform highp mat4 worldViewProjMatrix;								\n"\
	"uniform highp vec3 vSunDir;			// from 0,0,0 to sun position	\n"\
	"uniform highp vec4 vCameraPos;											\n"\
	"uniform mediump vec4 vWaveParams;										\n"\
	"uniform mediump vec2 vWaveDir;											\n"\

	"varying mediump vec2 vOutWave0;										\n"\
	"varying mediump vec2 vOutWave1;										\n"\
	"varying mediump vec2 vOutWave2;										\n"\
	"varying mediump vec2 vOutWave3;										\n"\
    "varying highp vec4 vOutScreenPos;										\n"\
 	"varying mediump vec3 vOutEye;											\n"\
    "varying mediump vec3 vOutLight;										\n"\
																			
	" void main()															\n"\
	" {																		\n"\
	"	highp mat3 objToTanMatrix;											\n"\
	"	objToTanMatrix[0] = vec3(1.0, 0.0, 0.0);							\n"\
	"	objToTanMatrix[1] = vec3(0.0, 0.0, 1.0);							\n"\
	"	objToTanMatrix[2] = vec3(0.0, 1.0, 0.0);							\n"\

	"	// vWaveParams.y is water height											\n"\
	"	highp vec3 inputPos = vec3(inPosition.x, vWaveParams.y, inPosition.z);		\n"\

	" 	gl_Position = worldViewProjMatrix * vec4(inputPos, 1.0);					\n"\

	"	mediump vec2 fTranslation = vec2(mod(vWaveParams.x, 100.0) * -vWaveDir);	\n"\
    "	mediump vec2 vTexCoords = inputPos.xz * vWaveParams.w;						\n"\

	"	// Scale texture coords to get mix of low/high frequency details	\n"\
    "	vOutWave0 = vTexCoords + fTranslation * 2.0;						\n"\
    "	vOutWave1 = vTexCoords * 2.0 + fTranslation * 4.0;					\n"\
    "	vOutWave2 = vTexCoords * 4.0 + fTranslation * 2.0;					\n"\
    "	vOutWave3 = vTexCoords * 8.0 + fTranslation;						\n"\
																			
	"	// perspective corrected projection									\n"\
    "	highp vec4 vHPos = gl_Position;										\n"\
	"	vOutScreenPos.xy = (vHPos.xy + vHPos.w) * 0.5;						\n"\
    "	vOutScreenPos.w = vHPos.w;											\n"\

	"	// vWaveParams.y is water height									\n"\
	"	// vWaveParams.z is scale											\n"\
	"	// vOutScreenPos.z is water's height - terrain's height				\n"\
	"	vOutScreenPos.z = (vWaveParams.y - inPosition.y) / vWaveParams.z;	\n"\

	"	mediump vec3 EyeVec = vCameraPos.xyz - inputPos.xyz;				\n"\
    "	vOutEye = normalize(objToTanMatrix * EyeVec);						\n"\
   																			
    "	//mediump vec3 LightVec = vLightPos.xyz - IN.Pos.xyz;				\n"\
	"	vOutLight = normalize(objToTanMatrix * vSunDir);					\n"\

	" }																		\n"\
	"";



char Shader_waterRender_PS_String [] =
	"																		\n"\
	"varying mediump vec2 vOutWave0;										\n"\
	"varying mediump vec2 vOutWave1;										\n"\
	"varying mediump vec2 vOutWave2;										\n"\
	"varying mediump vec2 vOutWave3;										\n"\
	"varying highp vec4 vOutScreenPos;										\n"\
	"varying mediump vec3 vOutEye;											\n"\
	"varying mediump vec3 vOutLight;										\n"\


	"uniform sampler2D gSampler0;		// Bumpmap wave Texture				\n"\
	"uniform sampler2D gSampler1;		// Refraction Texture				\n"\
	"uniform sampler2D gSampler2;		// Reflection Texture				\n"\

	"uniform mediump vec4 vWaterBumpFresnel;	// Bump and Fresnel Params				\n"\
	"uniform mediump vec2 vWaterDepthBlend;		// water depth Params					\n"\
	"uniform mediump vec4 vWaterColor;			// Water color(w is sun spec power)		\n"\
	"uniform lowp vec4 vSunColor;				// sun color for specular color			\n"\


	"// This assumes NdotL comes clamped																\n"\
	"mediump float Fresnel(mediump float NdotL, mediump float fresnelBias, mediump float fresnelPow)	\n"\
	"{																									\n"\
	"	mediump float facing = (1.0 - NdotL);															\n"\
	"	return max(fresnelBias + (1.0-fresnelBias) * pow(facing, fresnelPow), 0.0);						\n"\
	"}																									\n"\

	"void main()																									\n"\
	"{																												\n"\
	"	// Get bump layers																							\n"\
	"	mediump vec3 vBumpTexA = texture2D(gSampler0, vOutWave0).xyz;												\n"\
	"	mediump vec3 vBumpTexB = texture2D(gSampler0, vOutWave1).xyz;												\n"\
	"	mediump vec3 vBumpTexC = texture2D(gSampler0, vOutWave2).xyz;												\n"\
	"	mediump vec3 vBumpTexD = texture2D(gSampler0, vOutWave3).xyz;												\n"\
																										
	"	// Average bump layers																						\n"\
	"	mediump vec3 vBumpTex = normalize(2.0*(vBumpTexA + vBumpTexB + vBumpTexC + vBumpTexD)-4.0);					\n"\
																										
	"	// Apply individual bump scale for refraction and reflection												\n"\
	"	mediump vec3 vRefrBump = vBumpTex * vec3(vWaterBumpFresnel.x, vWaterBumpFresnel.x, 1.0);					\n"\
	"	mediump vec3 vReflBump = vBumpTex * vec3(vWaterBumpFresnel.y, vWaterBumpFresnel.y, 1.0); 					\n"\
																										
	"	// Compute projected coordinates																			\n"\
	"	highp vec2 vProj = vOutScreenPos.xy / vOutScreenPos.w;														\n"\
	"	lowp vec4 vReflection = texture2D(gSampler2, vProj.xy + vReflBump.xy);										\n"\
																										
	"	// Mask occluders from refraction map																		\n"\
	"	lowp vec4 vRefrA = texture2D(gSampler1, vProj.xy + vRefrBump.xy);											\n"\
	"	lowp vec4 vRefrB = texture2D(gSampler1, vProj.xy);															\n"\
																										
	"	// Mask occluders from refraction map																		\n"\
	"	lowp vec4 vRefraction = mix( vRefrA, vRefrB, vRefrA.w );													\n"\
																								
	"	// Compute specular highlight																				\n"\
	"	mediump vec3 NormalT = normalize( vReflBump );																\n"\
	"	mediump vec3 specular = vec3(0,0,0);																		\n"\
	"	if( vWaterColor.w > 0.0 )																					\n"\
	"	{																											\n"\
	"		mediump vec3 vRef = reflect(vOutEye, NormalT);															\n"\
	"		specular = pow(max(0.0, dot(vRef, -vOutLight)), vWaterColor.w) * vSunColor.rgb;							\n"\
	"	}																											\n"\
	
	"	// Compute Fresnel term																						\n"\
	"	mediump float NdotL = max(dot(vOutEye, NormalT), 0.0);														\n"\
	"	mediump float facing = (1.0 - NdotL);																		\n"\
	"	mediump float fresnel = Fresnel(NdotL, vWaterBumpFresnel.z, vWaterBumpFresnel.w);							\n"\

	"	// Use distance to lerp between refraction and deep water color												\n"\
	"	mediump float fDistScale = clamp(vWaterDepthBlend.x/(vOutScreenPos.z * vWaterDepthBlend.y), 0.0, 1.0);		\n"\
	"	lowp vec3 WaterDeepColor = mix( vWaterColor.xyz, vRefraction.xyz, fDistScale );								\n"\

	"	// Lerp between water color and deep water color															\n"\
	"	lowp vec3 waterColor = mix( WaterDeepColor, vWaterColor.xyz, facing );										\n"\
	"	lowp vec3 cReflect = fresnel * vReflection.xyz;																\n"\

    "	gl_FragColor = vec4( cReflect+waterColor+specular, clamp(vOutScreenPos.z*vWaterDepthBlend.y, 0.0, 1.0) );	\n"\
	"}																												\n"\

	"";