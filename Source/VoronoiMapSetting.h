#ifndef __VORONOI_MAPSETTING_HEADER__
#define __VORONOI_MAPSETTING_HEADER__

class CVoionoiMapSetting
{
public:
	enum EIslandShape
	{
		eRadialIsland,
		ePerlinIsland,
		eSquareIsland,
		eBlobIsland,
	};	

	enum ERandomGrid
	{
		eRelaxed,
		ePoissonDisk,
		eSquare,
		eHexagon,
	};

	enum ERenderMode
	{
		ePolygons,
		eBiome,
		eElevation,
		eMoisture,
		eSmoothPolygons,
		e2DSlopes,
		e3DSlopes,
	};

	enum ELineColor
	{
		eLineColorCOAST,			// 0x33335a
		eLineColorLAKESHORE,		// 0x225588
		eLineColorRIVER,			// RIVER: 0x225588

		eLineColorMax,
	};

	static Colour LineColour[eLineColorMax];
	static Colour SlopeColour[2];

public:
	uint32 uSitesNum;
	uint32 uImageWidth;			// 0-65535
	uint32 uImageHeight;		// 0-65535

	// Island details are controlled by this random generator. 
	// The initial map upon loading is always deterministic, but
    // subsequent maps reset this random number generator with a
    // random seed.
	Random detailRandom;
	uint32 detailSeed;

	Random islandRandom;
	uint32 islandSeed;

	Random* globalRandom;
	uint32 globalSeed;

	uint8			RelaxationNum;
	EIslandShape	IslandShapeType;
	ERandomGrid		RandomGridType;
	ERenderMode		RenderMode;

	unsigned char* image;
	uint32 imagesize;

	Vector3D lightDirection;

public:
	CVoionoiMapSetting() : image(NULL), imagesize(0), uSitesNum(500), uImageWidth(600), uImageHeight(600),
		RelaxationNum(0), globalRandom(NULL), globalSeed(0), detailSeed(0), islandSeed(0),
		IslandShapeType(eRadialIsland), RandomGridType(eRelaxed), RenderMode(ePolygons)
	{
		lightDirection.x = 1;
		lightDirection.y = 0;
		lightDirection.z = -1;
	}

	~CVoionoiMapSetting()
	{	
		if (image)
		{
			free(image);
			image = NULL;
		}
	}

	void SetGlobalRandom(Random* pRandom) { globalRandom = pRandom; }
	uint32 GetIslandRandomSeed() { return islandSeed; }
	void SetIslandRandomSeed(uint32 seed) 
	{ 
		islandSeed = seed; 
		globalSeed = seed;
		islandRandom.setSeed(seed);
		if (globalRandom)
			globalRandom->setSeed(seed); 
	}
	uint32 GetDetailRandomSeed() { return detailSeed; }
	void SetMapDetailRandomSeed(uint32 seed) { detailSeed = seed; detailRandom.setSeed(seed); }

	bool NeedMoreRandom() { return (RandomGridType == eSquare || RandomGridType == eHexagon); }
};

#endif