// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "../sgp_core/native/sgp_BasicNativeHeaders.h"
#include "sgp_world.h"

namespace sgp
{
	sgp_ImplementSingleton_SingleThreaded( CSGPWorldConfig );
	sgp_ImplementSingleton_SingleThreaded( CSGPLightMapGenConfig );

	#include "quadtree/sgp_QuadTree.cpp"
	#include "skydome/sgp_Skydome.cpp"
	#include "terrain/sgp_Terrain.cpp"
	#include "terrain/sgp_TerrainChunk.cpp"
	#include "grass/sgp_Grass.cpp"
	#include "world/sgp_WorldMap.cpp"	
}