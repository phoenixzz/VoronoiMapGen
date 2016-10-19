ISGPRenderDevice* ISGPMaterialProperty::m_pRenderDevice = NULL;


ISGPMaterialSystem::SGPMaterialInfo::SGPMaterialInfo()
{
	m_material = ISGPMaterialProperty::m_pRenderDevice->createMaterial();
}

ISGPMaterialSystem::SGPMaterialInfo::SGPMaterialInfo(char* MaterialStr )
{
	m_material = ISGPMaterialProperty::m_pRenderDevice->createMaterial(MaterialStr);
}

void ISGPMaterialSystem::SGPMaterialInfo::CloneMaterial( const SGPMaterialInfo& _MaterialInfo )
{ 
	m_material->Clone( _MaterialInfo.m_material );
}

void ISGPMaterialSystem::SGPMaterialInfo::Reset()
{
	if( m_material )
	{
		delete m_material;
		m_material = NULL;
	}
	m_material = ISGPMaterialProperty::m_pRenderDevice->createMaterial();
}

ISGPMaterialSystem::ISGPMaterialSystem(ISGPRenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
{
	ISGPMaterialProperty::m_pRenderDevice = m_pRenderDevice;

	m_materials.ensureStorageAllocated(INIT_MATERIALLIST_BEGIN_NUM);
	m_modifiers.ensureStorageAllocated(INIT_MATERIALLIST_BEGIN_NUM);
}

ISGPMaterialSystem::~ISGPMaterialSystem()
{
	for( int i=0; i<m_materials.size(); i++ )
	{
		if( m_materials[i].m_material )
		{
			delete m_materials[i].m_material;
			m_materials.getReference(i).m_material = NULL;
		}
	}
	for( int i=0; i<m_modifiers.size(); i++ )
	{
		if( m_modifiers[i].m_material )
		{
			delete m_modifiers[i].m_material;
			m_modifiers.getReference(i).m_material = NULL;
		}
	}
}


void ISGPMaterialSystem::AddMaterial( char* MaterialStr )
{
	m_materials.add( SGPMaterialInfo(MaterialStr) );
}

void ISGPMaterialSystem::AddModifier(  char* ModifierStr )
{
	m_modifiers.add( SGPMaterialInfo(ModifierStr) );
}

void ISGPMaterialSystem::AddMaterial( SGPMaterialInfo *pNewMaterialInfo )
{
	m_materials.add( *pNewMaterialInfo );
}

void ISGPMaterialSystem::AddModifier( SGPMaterialInfo *pNewModifierInfo )
{
	m_modifiers.add( *pNewModifierInfo );
}

void ISGPMaterialSystem::UpdateDynamicMaterials( float elapsedTime )
{
	AbstractMaterial::UpdateDynamicMaterials( elapsedTime );
}

void ISGPMaterialSystem::LoadGameMaterials()
{
	// Materials
	#include "MaterialString/opaque_base.h"
	AddMaterial( Material_opaque_base_String );
	#include "MaterialString/transparent.h"
	AddMaterial( Material_transparent_String );
	#include "MaterialString/alphatest.h"
	AddMaterial( Material_alphatest_String );

	#include "MaterialString/particle_addalpha.h"
	AddMaterial( Material_particleaddalpha_String );
	#include "MaterialString/particle_addalpha_line.h"
	AddMaterial( Material_particleaddalpha_Line_String );

	#include "MaterialString/terrain.h"
	AddMaterial( Material_terrain_String );

	#include "MaterialString/skydome.h"
	AddMaterial( Material_skydome_String );

	#include "MaterialString/line.h"
	AddMaterial( Material_Line_String );
	#include "MaterialString/Font.h"
	AddMaterial( Material_Font_String );

	#include "MaterialString/fullscreenQuad.h"
	AddMaterial( Material_fullscreenquad_String );

	#include "MaterialString/water_refraction.h"
	AddMaterial( Material_waterRefraction_String );
	#include "MaterialString/water_render.h"
	AddMaterial( Material_waterRender_String );

	#include "MaterialString/grass.h"
	AddMaterial( Material_grass_String );

	// Modifiers
	#include "ModifierString/nodepth.h"
	AddModifier( Modifier_NoDepth_String );
	#include "ModifierString/depthbias.h"
	AddModifier( Modifier_DepthBias_String );
}

