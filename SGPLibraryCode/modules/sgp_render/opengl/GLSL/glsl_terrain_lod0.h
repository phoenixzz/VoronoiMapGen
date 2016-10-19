

char Shader_terrain_LOD0_VS_String[] = 
	"#version 330													\n"\
	"																\n"\
	"layout (location = 0) in vec4 inPosition;						\n"\
	"layout (location = 1) in vec3 inNormal;						\n"\
	"layout (location = 2) in vec2 inCoord0;						\n"\
	"layout (location = 3) in vec2 inCoord1;						\n"\
	"layout (location = 4) in vec3 inTangent;						\n"\
	"layout (location = 5) in vec3 inBinormal;						\n"\
	"																\n"\
	"uniform mat4 worldViewProjMatrix;								\n"\
	"uniform float fFarPlane;										\n"\
	"																\n"\
	"out vec3 vNormal;												\n"\
	"out vec2 vTexCoord0;											\n"\
	"out vec2 vTexCoord1;											\n"\
	"out vec4 vOutWorldPos;			// z channel save as depth		\n"\
	"																\n"\
	" void main()													\n"\
	" {																\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.xyz, 1.0);		\n"\
	"																		\n"\
	"	// Calculate the normal vector against the world matrix only and	\n"\
	"	// then normalize the final value.									\n"\
	"	//vNormal = mat3x3(worldMatrix) * inNormal;							\n"\
	"	vNormal = normalize(inNormal);										\n"\
	"																		\n"\
	" 	vTexCoord0 = inCoord0;												\n"\
	"	vTexCoord1 = inCoord1;												\n"\
	"	vOutWorldPos = vec4(inPosition.xyz, gl_Position.z/gl_Position.w);	\n"\
	"	// store Depth into vOutWorldPos z channel as current w / farplane	\n"\
	"	vOutWorldPos.z = gl_Position.w / fFarPlane;							\n"\
	" }																		\n"\
	"";

char Shader_terrain_LOD0_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"in vec3 vNormal;														\n"\
	"in vec2 vTexCoord0;													\n"\
	"in vec2 vTexCoord1;													\n"\
	"in vec4 vOutWorldPos;													\n"\

	"layout (location = 0) out vec4 outputColor;							\n"\
	"layout (location = 1) out vec4 outputColor1;							\n"\

	"uniform sampler2D gSamplerDiffuse0;									\n"\
	"uniform sampler2D gSamplerDiffuse1;									\n"\
	"uniform sampler2D gSamplerDiffuse2;									\n"\
	"uniform sampler2D gSamplerDiffuse3;									\n"\
	"uniform sampler2D gSamplerAlphaBlend;									\n"\
	"uniform sampler2D gSamplerSlope;										\n"\
	"uniform sampler2D gSamplerLightmap;									\n"\

	"uniform vec4 SunColor;													\n"\
    "uniform vec3 SunDirection;												\n"\
	"uniform int RenderFlag;												\n"\

	"void main()															\n"\
	"{																		\n"\
	"	vec4 Diffuse1, Diffuse2, Diffuse3;									\n"\
	"	vec4 blendValue = texture2D(gSamplerAlphaBlend, vTexCoord1);		\n"\
	"	vec4 Diffuse0 = texture2D(gSamplerDiffuse0, vTexCoord0);			\n"\
	"	// Set the base color to the first color texture.					\n"\
	"	vec4 DiffuseColor = Diffuse0;										\n"\

	"	if( mod( RenderFlag, 32 ) >= 16 )									\n"\
	"	{																	\n"\
	"		Diffuse1 = texture2D(gSamplerDiffuse1, vTexCoord0);				\n"\
	"		// Add the second layer using the red channel of the alpha map.	\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse1, blendValue.x);		\n"\
	"	}																	\n"\
	"	if( mod( RenderFlag, 64 ) >= 32 )									\n"\
	"	{																	\n"\
	"		Diffuse2 = texture2D(gSamplerDiffuse2, vTexCoord0);				\n"\
	"		// Add the third layer using the green channel of the alpha map.\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse2, blendValue.y);		\n"\
	"	}																	\n"\
	"	if( mod( RenderFlag, 128 ) >= 64 )									\n"\
	"	{																	\n"\
	"		Diffuse3 = texture2D(gSamplerDiffuse3, vTexCoord0);				\n"\
	"		// Add the fourth layer using the blue channel of the alpha map.\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse3, blendValue.z);		\n"\
	"	}																	\n"\
																					
	"																				\n"\
	"	// Have Slope map?															\n"\
	"	if( mod( RenderFlag, 4 ) >= 2 )												\n"\
	"	{																			\n"\
	"		float slope = 1.0 - vNormal.y;											\n"\
	"	    vec2 vSlopeTexCoord = vTexCoord0.xy;									\n"\
	"		// Using Triplanar Texturing											\n"\
	"		if( mod( RenderFlag, 8 ) >= 4 )											\n"\
	"		{																		\n"\
	"			// Determine which texture axis to use based on normal pointing to.	\n"\
	"			if( abs(vNormal.x) >= abs(vNormal.z) )								\n"\
	"			{																	\n"\
	"				vSlopeTexCoord.x = vOutWorldPos.y / 10.0;						\n"\
	"			}																	\n"\
	"			else																\n"\
	"			{																	\n"\
	"				vSlopeTexCoord.y = vOutWorldPos.y / 10.0;						\n"\
	"			}																	\n"\
	"		}																		\n"\
	"		vec4 slopeColor = texture2D(gSamplerSlope, vSlopeTexCoord);				\n"\
	"		// Determine which texture to use based on height.						\n"\
	"		if(slope > 0.4)															\n"\
	"		{																		\n"\
	"			float blendAmount = (slope - 0.4) / 0.6;							\n"\
	"			DiffuseColor = mix(DiffuseColor, slopeColor, blendAmount);			\n"\
	"		}																		\n"\
	"	}																			\n"\
	"																				\n"\
	"	// Invert the light direction for calculations.								\n"\
    "	// Calculate the light based on the bump map normal value.					\n"\
    "	float lightIntensity = clamp(dot(vNormal, -SunDirection), 0.0, 1.0);		\n"\

	"	// lightmap color															\n"\
	"	vec4 lightmap = texture2D(gSamplerLightmap, vTexCoord1);					\n"\

    "	// Determine the final diffuse color based on the diffuse color and			\n"\
	"	// the amount of light intensity.											\n"\
    "	outputColor = DiffuseColor * lightIntensity * SunColor * lightmap.a;		\n"\
	"	outputColor.rgb += DiffuseColor.rgb * lightmap.rgb;							\n"\
	"	outputColor = clamp(outputColor, 0.0, 1.0);									\n"\
	"	outputColor.w = 1.0;														\n"\

	"	// Packing a [0-1] float depth value into a 4D vector						\n"\
	"	// where each component will be 8-bits color value							\n"\
	"	const vec4 bitSh = vec4(16777216.0, 65536.0, 256.0, 1.0);					\n"\
	"	const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);				\n"\
	"	outputColor1 = fract(vOutWorldPos.z * bitSh);								\n"\
	"	outputColor1 -= outputColor1.xxyz * bitMsk;									\n"\
	"																				\n"\
	"}																				\n"\

	"";