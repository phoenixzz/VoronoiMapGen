#ifndef __SGP_PARTICLE_PARAM_DEF_HEADER__
#define __SGP_PARTICLE_PARAM_DEF_HEADER__

#pragma pack(push, packing)
#pragma pack(1)

/**************************************************************
Model Info
**************************************************************/
// Interpolator Type
enum ParticleInterpolatorType
{
	Interpolator_SelfDefine,
	Interpolator_Sinusoidal,
	Interpolator_Polynomial
};

//Interpolator Entry Param define
struct ParticleEntryParam
{
	float m_x;
	float m_y0;
	float m_y1;
	ParticleEntryParam(float x=0,float y0=0,float y1=0)
	{
		this->m_x=x;
		this->m_y0=y0;
		this->m_y1=y1;
	}

};

// Self Define Interpolator
struct ParticleSelfDefInterpolatorData
{
	uint32 m_count;
	ParticleEntryParam* m_pEntry;
};

// Sinusoidal Interpolator
struct ParticleSinInterpolatorData
{
	float m_period;
	float m_amplitudeMin;
	float m_amplitudeMax;
	float m_offsetX;
	float m_offsetY;
	float m_startX;
	uint32 m_length;
	uint32 m_nbSamples;
};

// Polynomial Interpolator
struct ParticlePolyInterpolatorData
{
	float m_constant;
	float m_linear;
	float m_quadratic;
	float m_cubic;
	float m_startX;
	float m_endX;
	uint32 m_nbSamples;
};

struct ParticleInterpolatorParam
{
	uint32 m_ModelParam;
	ParticleInterpolatorType m_InterpolatorType;
	union
	{
		ParticleSelfDefInterpolatorData m_SelfDefData;
		ParticleSinInterpolatorData m_SinData;
		ParticlePolyInterpolatorData m_PolyData;
	};

	ParticleInterpolatorParam()
	{
		m_SelfDefData.m_count=0;
		m_SelfDefData.m_pEntry=NULL;
	}

	~ParticleInterpolatorParam()
	{
		if(m_InterpolatorType==Interpolator_SelfDefine && m_SelfDefData.m_pEntry!=NULL)
		{
			delete [] m_SelfDefData.m_pEntry;
			m_SelfDefData.m_pEntry=NULL;
		}
	}
};

struct ParticleRegularParam
{
	uint8 m_ModelParam;
	uint8 m_count;
	float m_data[4];
};

struct ParticleModelParam
{
	float m_LifeTimeMin;
	float m_LifeTimeMax;
	bool m_bImmortal;

	uint32 m_EnableFlag;
	uint32 m_MutableFlag;
	uint32 m_RandomFlag;
	uint32 m_InterpolatedFlag;
	
	uint32 m_ParamCount;
	ParticleRegularParam *m_pRegularParam;
	uint32 m_InterpolatorCount;
	ParticleInterpolatorParam* m_pInterpolatorParam;

	ParticleModelParam()
	{
		m_ParamCount=0;
		m_pRegularParam=NULL;
		m_InterpolatorCount=0;
		m_pInterpolatorParam=NULL;
	}
	~ParticleModelParam()
	{
		if(m_pRegularParam!=NULL)
		{
			delete [] m_pRegularParam;
			m_pRegularParam=NULL;
		}
		if(m_pInterpolatorParam!=NULL)
		{
			delete [] m_pInterpolatorParam;
			m_pInterpolatorParam=NULL;
		}
	}
};

/**************************************************************
Render Info:
configuration for SPARKOpenGLPointRenderer,SPARKOpenGLLineRenderer,SPARKOpenGLQuadRenderer class
**************************************************************/
enum ParticleRenderType
{
	Render_Point,// SPARKOpenGLPointRenderer
	Render_Line,// SPARKOpenGLLineRenderer
	Render_Quad// SPARKOpenGLQuadRenderer
};

struct ParticlePointRenderData//SPARKOpenGLPointRenderer Configure Param data
{

	float m_size;
	char m_texPath[64];
	uint8 m_type;//PointType
};

struct ParticleLineRenderData//SPARKOpenGLLineRenderer data
{
	float m_length;
	float m_width;
};

struct ParticleQuadRenderData//SPARKOpenGLQuadRenderer data
{
	float m_xScale;
	float m_yScale;
	char m_texPath[64];
	int32 m_xDimension;
	int32 m_yDimension;
	int32 m_nOrientation;
	float m_lookVector[3];
	float m_upVector[3];
	uint8 m_texMode;//texture mode	
};

struct ParticleRenderParam
{
	ParticleRenderType m_type;
	bool m_enableRenderHint;
	float m_alphaTestThreshold;
	uint8 m_blendMode;//BlendingMode
	union
	{
		ParticlePointRenderData m_pointData;
		ParticleLineRenderData m_lineData;
		ParticleQuadRenderData m_quadData;
	};

	ParticleRenderParam(const ParticlePointRenderData& pointData)
	{
		m_pointData=pointData;
		m_type=Render_Point;
	}
	ParticleRenderParam(const ParticleLineRenderData& lineData)
	{
		m_lineData=lineData;
		m_type=Render_Line;
	}
	ParticleRenderParam(const ParticleQuadRenderData& quadData)
	{
		m_quadData=quadData;
		m_type=Render_Quad;
	}
	ParticleRenderParam() {}
};
/**************************************************************************
Emiter Info
**************************************************************************/
/********************************************
	zone info
********************************************/
enum ParticleZoneType
{
	Zone_Sphere,//SphereZone
	Zone_AABox,//AABoxZone
	Zone_Line,//LineZone
	Zone_Plane,//PlaneZone
	Zone_Ring,//RingZone
	Zone_Point//PointZone
};

// for SphereZone configuration
struct ParticleSphereZoneData
{
	float m_position[3];
	float m_radius;

	void SetValue(float x,float y,float z,float radius)
	{
		m_position[0]=x;
		m_position[1]=y;
		m_position[2]=z;
		m_radius=radius;
	}
};

// for AABoxZone configuration
struct ParticleAABoxZoneData
{
	float m_position[3];
	float m_dimension[3];

	void SetValue(float x,float y,float z,float cx,float cy,float cz)
	{
		m_position[0]=x;
		m_position[1]=y;
		m_position[2]=z;
		m_dimension[0]=cx;
		m_dimension[1]=cy;
		m_dimension[2]=cz;
	}
};

// for LineZone configuration
struct ParticleLineZoneData
{
	float m_p0[3];
	float m_p1[3];

	void SetValue(float x1,float y1,float z1,float x2,float y2,float z2)
	{
		m_p0[0]=x1;
		m_p0[1]=y1;
		m_p0[2]=z1;
		m_p1[0]=x2;
		m_p1[1]=y2;
		m_p1[2]=z2;
	}
};

// for PlaneZone configuration
struct ParticlePlaneZoneData
{
	float m_position[3];
	float m_normal[3];

	void SetValue(float x,float y,float z,float nx,float ny,float nz)
	{
		m_position[0]=x;
		m_position[1]=y;
		m_position[2]=z;
		m_normal[0]=nx;
		m_normal[1]=ny;
		m_normal[2]=nz;
	}
};

// for RingZone configuration
struct ParticleRingZoneData
{
	float m_position[3];
	float m_normal[3];
	float m_minRadius;
	float m_maxRadius;

	void SetValue(float x,float y,float z,float nx,float ny,float nz,float minRadius,float maxRadius)
	{
		m_position[0]=x;
		m_position[1]=y;
		m_position[2]=z;
		m_normal[0]=nx;
		m_normal[1]=ny;
		m_normal[2]=nz;
		m_minRadius=minRadius;
		m_maxRadius=maxRadius;
	}
};

// for PointZone configuration
struct ParticlePointZoneData
{
	float m_position[3];

	void SetValue(float x,float y,float z)
	{
		m_position[0]=x;
		m_position[1]=y;
		m_position[2]=z;
	}
};

struct ParticleZoneParam
{
	ParticleZoneType m_ZoneType;//zone type
	bool m_bFullZone;
	union
	{
		ParticleSphereZoneData m_sphereZoneData;
		ParticleAABoxZoneData m_aaboxZoneData;
		ParticleLineZoneData m_lineZoneData;
		ParticlePlaneZoneData m_planeZoneData;
		ParticleRingZoneData m_ringZoneData;
		ParticlePointZoneData m_pointZoneData;
	};
};

/********************************************
	emitter info
********************************************/
enum ParticleEmitterType
{
	Emitter_Straight,//StraightEmitter
	Emitter_Static,//StaticEmitter
	Emitter_Spheric,//SphericEmitter
	Emitter_Random,//RandomEmitter
	Emitter_Normal//NormalEmitter
};

// for StraightEmitter configuration
struct ParticleStraightEmitterData
{
	float m_direction[3];

	void SetValue(float x,float y,float z)
	{
		m_direction[0]=x;
		m_direction[1]=y;
		m_direction[2]=z;
	}
};

// for StaticEmitter configuration
struct ParticleStaticEmitterData
{
	float NoUse;
};

// for SphericEmitter configuration
struct ParticleSphericEmitterData
{
	float m_direction[3];
	float m_angleA;
	float m_angleB;

	void SetValue(float x,float y,float z,float angleA,float angleB)
	{
		m_direction[0]=x;
		m_direction[1]=y;
		m_direction[2]=z;
		m_angleA=angleA;
		m_angleB=angleB;
	}
};

// for RandomEmitter configuration
struct ParticleRandomEmitterData
{
	float NoUse;
};

// for NormalEmitter configuration
struct ParticleNormalEmitterData
{
	bool m_bInverted;

	void SetValue(bool inverted)
	{
		m_bInverted=inverted;
	}
};

// emitter param definition
struct ParticleEmitterParam
{
	ParticleEmitterType m_EmitterType;//emitter type
	union
	{
		ParticleStraightEmitterData m_straightEmitterData;
		ParticleStaticEmitterData m_staticEmitterData;
		ParticleSphericEmitterData m_sphericEmitterData;
		ParticleRandomEmitterData m_randomEmitterData;
		ParticleNormalEmitterData m_normalEmitterData;
	};
	ParticleZoneParam m_zoneParam;
	float m_Flow;
	int32 m_Tank;
	float m_ForceMin;
	float m_ForceMax;

};

/*******************************************
	Modifier Info
*******************************************/
enum ParticleModifierType
{
	Modifier_Obstacle,
	Modifier_LinearForce,
	Modifier_PointMass
};

struct ParticleObstacleData
{
	float m_bounceRatio;
	float m_Friction;
};

struct ParticleLinearForceData
{
	float m_Force[3];
	uint8 m_ForceFactorType;
	uint8 m_FactorParam;
};

struct ParticlePointMassData
{
	float m_Position[3];
	float m_Mass;
	float m_MinDistance;
};

struct ParticleModifierParam
{
	ParticleModifierType m_ModifierType;
	union
	{
		ParticleObstacleData m_ObstacleData;
		ParticleLinearForceData m_LinearForceData;
		ParticlePointMassData m_PointMassData;
	};
	bool m_bCustomZone;
	ParticleZoneParam m_ZoneParam;
};

/***************************************************************
	Group Info
***************************************************************/
struct ParticleGroupParam
{
	char m_GroupName[16];
	bool m_EnableAABBComputing;
	bool m_EnableComputeDistance;
	bool m_EnableSorting;
	float m_Gravity[3];//gravity
	uint32 m_Capacity;
	float m_Friction;
	ParticleModelParam m_ModelParam;
	ParticleRenderParam m_RenderParam;
	uint32 m_nEmitterCount;
	ParticleEmitterParam *m_pEmitterParam;
	uint32 m_nModifierCount;
	ParticleModifierParam *m_pModifierParam;

	ParticleGroupParam()
	{
		m_nEmitterCount=0;
		m_pEmitterParam=NULL;
		m_nModifierCount=0;
		m_pModifierParam=NULL;
		memset(m_GroupName,0,16);
	}
	~ParticleGroupParam()
	{
		if(m_pEmitterParam!=NULL)
		{
			delete [] m_pEmitterParam;
			m_pEmitterParam=NULL;
		}
		if(m_pModifierParam!=NULL)
		{
			delete [] m_pModifierParam;
			m_pModifierParam=NULL;
		}
	}
};

/****************************************************************
	System Info
****************************************************************/
struct ParticleSystemParam
{
	bool m_bEnableAABBCompute;
	uint32 m_groupCount;
	ParticleGroupParam* m_pGroupParam;

	ParticleSystemParam()
	{
		m_groupCount=0;
		m_pGroupParam=NULL;
	}
	~ParticleSystemParam()
	{
		if(m_pGroupParam!=NULL)
		{
			delete [] m_pGroupParam;
			m_pGroupParam=NULL;
		}
	}
};

#pragma pack(pop, packing)

#endif
