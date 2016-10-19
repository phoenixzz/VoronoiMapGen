#ifndef __SGP_MODELFILEMESHCONFIGSETTING_HEADER__
#define __SGP_MODELFILEMESHCONFIGSETTING_HEADER__

#pragma pack(push, packing)
#pragma pack(1)

struct SGPMF1ConfigSetting
{
	struct MeshConfig
	{
		uint32 MeshID;
		bool bVisible;
		MeshConfig() : MeshID(0xFFFFFFFF), bVisible(true) {}
	};
	struct ReplaceTextureConfig
	{
		uint32 MeshID;
		char TexFilename[64];
		ReplaceTextureConfig() : MeshID(0xFFFFFFFF) { memset(TexFilename, 0, 64); }
	};
	struct ParticleConfig
	{
		uint32 ParticleID;
		bool bVisible;
		ParticleConfig() : ParticleID(0xFFFFFFFF), bVisible(true) {}
	};
	struct RibbonConfig
	{
		uint32 RibbonID;
		bool bVisible;
		RibbonConfig() : RibbonID(0xFFFFFFFF), bVisible(true) {}
	};

	uint32 MeshConfigNum;				// Number of Mesh setting for this MF1 config
	uint32 ReplaceTextureConfigNum;		// Number of tex setting for this MF1 config
	uint32 ParticleConfigNum;			// Number of particle setting for this MF1 config
	uint32 RibbonConfigNum;				// Number of ribbon setting for this MF1 config

	MeshConfig *pMeshConfigList;						// Mesh Config List
	ReplaceTextureConfig *pReplaceTextureConfigList;	// ReplaceTexture Config List
	ParticleConfig *pParticleConfigList;				// Particle Config List
	RibbonConfig *pRibbonConfigList;					// Ribbon Config List

	SGPMF1ConfigSetting() : MeshConfigNum(0), ReplaceTextureConfigNum(0), ParticleConfigNum(0), RibbonConfigNum(0)
	{
		pMeshConfigList = NULL;
		pReplaceTextureConfigList = NULL;
		pParticleConfigList = NULL;
		pRibbonConfigList = NULL;
	}
	~SGPMF1ConfigSetting()
	{
		if( pMeshConfigList )
		{
			delete [] pMeshConfigList;
			pMeshConfigList = NULL;
		}
		if( pReplaceTextureConfigList )
		{
			delete [] pReplaceTextureConfigList;
			pReplaceTextureConfigList = NULL;
		}
		if( pParticleConfigList )
		{
			delete [] pParticleConfigList;
			pParticleConfigList = NULL;
		}
		if( pRibbonConfigList )
		{
			delete [] pRibbonConfigList;
			pRibbonConfigList = NULL;
		}
	}
};

#pragma pack(pop, packing)

#endif		//__SGP_MODELFILEMESHCONFIGSETTING_HEADER__