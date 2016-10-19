const char *Shader_hoffmanskydome_Attribute_String[] = { "inPosition" };

char Shader_hoffmanskydome_VS_String[] = 
	"																				\n"\
	"attribute highp vec3 inPosition;												\n"\

	"uniform highp mat4 modelViewProjMatrix;										\n"\
	"uniform highp mat4 modelViewMatrix;											\n"\
	"uniform mediump vec3 InvSunDir;												\n"\
	"uniform highp vec3 BetaRPlusBetaM;												\n"\
																			
	"varying mediump vec2 vTexCoordPass;											\n"\
	"varying highp vec3 vInPosition;												\n"\
	"varying mediump vec3 vSunInvDirection;											\n"\
	"varying highp vec3 vOutExtinction;												\n"\

	" void main()																	\n"\
	" {																				\n"\
	" 	 highp vec4 position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	"	 gl_Position = position.xyww;												\n"\
	" 	 vTexCoordPass.xy = inPosition.xz;											\n"\
	"	 vInPosition = inPosition;													\n"\
	
	"	 // Extinction term															\n"\
	"	 highp vec4 vertex = modelViewMatrix * vec4(inPosition, 1.0);				\n"\
	"	 highp float distance = length( vertex.xyz ) * 1000.0;  					\n"\
	"	 vOutExtinction = exp( -BetaRPlusBetaM * distance );						\n"\

	"	 mediump vec4 sunInvDirection = modelViewMatrix * vec4(InvSunDir, 1.0);		\n"\
	"	 vSunInvDirection = normalize(sunInvDirection.xyz);							\n"\

	" }																				\n"\
	"";


char Shader_hoffmanskydome_PS_String[] = 
	"																				\n"\
	"varying mediump vec2 vTexCoordPass;											\n"\
	"varying highp vec3 vInPosition;												\n"\
	"varying mediump vec3 vSunInvDirection;											\n"\
	"varying highp vec3 vOutExtinction;												\n"\


	"uniform sampler2D gSampler0;		// Cloud texture							\n"\
	"uniform sampler2D gSampler1;		// Noise texture							\n"\
	"uniform mediump vec4 cloudParam;	// cloud movement param						\n"\

	"uniform highp mat4 modelViewMatrix;											\n"\
	"uniform highp vec3 HGg;														\n"\
	"uniform highp vec4 Multipliers;												\n"\
	"uniform highp vec3 BetaDashR;													\n"\
	"uniform highp vec3 BetaDashM;													\n"\
	"uniform highp vec3 OneOverBetaRPlusBetaM;										\n"\
	"uniform mediump vec4 SunColorAndIntensity;										\n"\

	"void main()																	\n"\
	"{																				\n"\
	"	mediump vec2 coludUV = vTexCoordPass;										\n"\
	"	mediump vec4 perturbValue;													\n"\

	"	highp vec4 vertex = modelViewMatrix * vec4(vInPosition, 1.0);				\n"\
	"	highp vec3 vViewDirection = normalize(vertex.xyz);							\n"\

	"	highp float theta = dot( vSunInvDirection, vViewDirection );									\n"\
	"	// Phase1 and Phase2																			\n"\
	"	highp float phase1 = 1.0 + theta * theta;														\n"\
	"	highp float phase2 = pow( inversesqrt(HGg.y - HGg.z * theta), 3.0 ) * HGg.x;					\n"\

	"	// Inscattering term																			\n"\
	"	highp vec3 betaRay = BetaDashR * phase1;														\n"\
	"	highp vec3 betaMie = BetaDashM * phase2;														\n"\
	"	highp vec3 inscatter = (betaRay + betaMie) * OneOverBetaRPlusBetaM * (1.0 - vOutExtinction);	\n"\
																								
	"	// Apply inscattering contribution factors									\n"\
	"	inscatter *= Multipliers.x;													\n"\
	
	"	// Scale with sun color & intensity											\n"\
	"	inscatter *= SunColorAndIntensity.xyz * 100.0;								\n"\

	"	gl_FragColor = vec4(inscatter, 1.0);										\n"\

	"	// Translate the texture coordinate sampling location						\n"\
	"	// by the translation value.												\n"\
    "	coludUV.x = coludUV.x + cloudParam.x;										\n"\

    "	// Sample the texture value from the noise texture using					\n"\
	"	// the translated texture coordinates.										\n"\
	"	// Also multiply the perturb value by the perturb scale.					\n"\
    "	perturbValue = texture2D(gSampler1, coludUV) * cloudParam.z;				\n"\

    "	// Add the texture coordinates as well as the translation value				\n"\
	"	// to get the perturbed texture coordinate sampling location.				\n"\
    "	perturbValue.xy = perturbValue.xy + coludUV.xy + cloudParam.xy;				\n"\

	"   // Now sample the color from the cloud texture using						\n"\
	"	// the perturbed sampling coordinates.										\n"\
	"	// Also alpha blend the cloud color with the brightness value.				\n"\
	"	lowp vec4 cloudcolor = texture2D(gSampler0, perturbValue.xy);				\n"\
    "	gl_FragColor.rgb += gl_FragColor.rgb * (1.0-cloudcolor.w*cloudParam.w) + cloudcolor.xyz * cloudParam.w * cloudcolor.w;	\n"\
	"}																				\n"\

	"";