#ifndef __SGP_WORLD_HEADER__
#define __SGP_WORLD_HEADER__

#include "../sgp_core/sgp_core.h"
#include "../sgp_math/sgp_math.h"
#include "../sgp_model/sgp_model.h"

namespace sgp
{
#ifndef __SGP_OBJECT_HEADER__
	#include "sceneobject/sgp_Object.h"
#endif
#ifndef __SGP_LIGHT_HEADER__
	#include "sceneobject/sgp_Light.h"
#endif
#ifndef __SGP_WORLDSUN_HEADER__
	#include "scattering/sgp_WorldSun.h"
#endif
#ifndef __SGP_ATMOSPHERICSCATTERING_HEADER__
	#include "scattering/sgp_HoffmanPreethem.h"
#endif

#ifndef __SGP_SKYDOME_HEADER__
	#include "skydome/sgp_Skydome.h"
#endif

#ifndef __SGP_TERRAIN_HEADER__
	#include "terrain/sgp_Terrain.h"
#endif
#ifndef __SGP_TERRAINCHUNK_HEADER__
	#include "terrain/sgp_TerrainChunk.h"
#endif
#ifndef __SGP_TERRAINTILESHAPE_HEADER__
	#include "terrain/sgp_TerrainTileShape.h"
#endif

#ifndef __SGP_QUADTREE_HEADER__
	#include "quadtree/sgp_QuadTree.h"
#endif

#ifndef __SGP_WATER_HEADER__
	#include "water/sgp_Water.h"
#endif

#ifndef __SGP_GRASS_HEADER__
	#include "grass/sgp_Grass.h"
#endif

#ifndef __SGP_WORLDCONFIG_HEADER__
	#include "world/sgp_WorldConfig.h"
#endif
#ifndef __SGP_WORLDMAP_HEADER__
	#include "world/sgp_WorldMap.h"
#endif

#ifndef __SGP_LIGHTMAPGENCONFIG_HEADER__
	#include "world/sgp_LightmapGenConfig.h"
#endif



}

#endif		// __SGP_WORLD_HEADER__