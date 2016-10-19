#ifndef __SGP_MATERIALSYSTEM_HEADER__
#define __SGP_MATERIALSYSTEM_HEADER__


class ISGPMaterialSystem
{
public:
	enum GameMaterial
	{
		eMaterial_opaque_base = 0,
		eMaterial_transparent,
		eMaterial_alphatest,

		eMaterial_particleaddalpha,
		eMaterial_particleaddalpha_line,

		eMaterial_terrain,
		eMaterial_skydome,

		eMaterial_line,
		eMaterial_font,

		eMaterial_fullscreenquad,

		eMaterial_waterrefraction,
		eMaterial_waterrender,

		eMaterial_grass,


		eMaterial_Unknow,
	};

	enum GameModifier
	{
		eModifier_nodepth = 0,
		eModifier_depthbias,

		eModifier_Unknow,
	};

	struct SGPMaterialInfo
	{
		SGPMaterialInfo();
		SGPMaterialInfo( char* MaterialStr );
		void CloneMaterial( const SGPMaterialInfo& _MaterialInfo );
		void Reset();
		AbstractMaterial *m_material;
	};

	typedef Array<SGPMaterialInfo>	MaterialList;

public:
	static const int INIT_MATERIALLIST_BEGIN_NUM = 32;

	ISGPMaterialSystem(ISGPRenderDevice* pRenderDevice);
	~ISGPMaterialSystem();

	void	AddMaterial( char* MaterialStr );
	void	AddModifier( char* ModifierStr );
	void	AddMaterial( struct SGPMaterialInfo *pNewMaterialInfo );
	void	AddModifier( struct SGPMaterialInfo *pNewModifierInfo );

	void	UpdateDynamicMaterials( float elapsedTime );
	void	LoadGameMaterials();

	// Interface
	inline	MaterialList& GetMaterialList() { return m_materials; }
	inline	MaterialList& GetModifierList() { return m_modifiers; }

private:
	MaterialList	m_materials;
	MaterialList	m_modifiers;
	ISGPRenderDevice* m_pRenderDevice;
};



#endif		// __SGP_MATERIALSYSTEM_HEADER__