#ifndef __MODEL_CENTER_HEADER__
#define __MODEL_CENTER_HEADER__


class Edge;
class Corner;

class Center
{
public:
	enum EBiome
	{
		eBiomeOcean,
		eBiomeMarsh,
		eBiomeLake,
		eBiomeIce,
		eBiomeBeach,
		eBiomeSnow,
		eBiomeTundra,
		eBiomeBare,
		eBiomeScorched,
		eBiomeTaiga,

		eBiomeShrubland,
		eBiomeTemperateDesert,
		eBiomeTemperateRainForest,
		eBiomeTemperateDeciduousForest,
		eBiomeGrassland,
		eBiomeTropicalRainForest,
		eBiomeTropicalSeasonalForest,
		eBiomeSubtropicalDesert,

		eBiomeMax,
	};

	static Colour BiomeColour[eBiomeMax];

public:
	uint32 Index;
	uint32 Key;							// hashed key
	Vector2D Point;						// location
	bool bWater;						// lake or ocean
	bool IsLand() { return !bWater; }
	void SetLand(bool value) { bWater = !value; }
	bool bOcean;						// is ocean?
	bool bCoast;						// land polygon touching an ocean
	bool bBorder;						// at the edge of the map
	Vector3D PolyNormal;				// Poly's Normal, for render
	Colour PolyColor;					// Poly's Color, for render

	float fElevation;					// 0.0 - 1.0
	float fMoisture;					// 0.0 - 1.0

	EBiome Biome;

	Array<Center*> Neighbours;
	Array<Edge*> Borders;
	Array<Corner*> Corners;

public:
    Center(float x, float y)
    {
        PolyNormal.x = PolyNormal.y = PolyNormal.z = 0;
        Point.x = x;
		Point.y = y;
        CreateKey();
            
        bWater = bCoast = bOcean = bBorder = false;
        fElevation = fMoisture = 0.0f;
		Index = 0;
    }

	bool IsEquals(const Center& other)
    {
		return (Point.x == other.Point.x) && (Point.y == other.Point.y);
	}

	uint32 CreateKey()
	{
		Key = ((((uint32)Point.x) & 0xFFFF) << 16) | (((uint32)Point.y) & 0xFFFF);
		return Key;
	}

	String GetBiomeName();

	static Colour GetBiomeColor(float fElevation, float fMoisture);
	

public:
	void OrderCorners();
	void FixBorders();
	void ResetBiome();	
};


#endif