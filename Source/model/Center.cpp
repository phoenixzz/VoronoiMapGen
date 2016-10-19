

#include "../SGPLibraryCode/SGPHeader.h"

#include "Center.h"
#include "Corner.h"
#include "Edge.h"

Colour Center::BiomeColour[eBiomeMax] = {
		Colour(68, 68, 122),		// OCEAN: 0x44447a,
		Colour(47, 102, 102),		// MARSH: 0x2f6666
		Colour(51, 51, 153),		// LAKE: 0x336699
		Colour(153, 255, 255),		// ICE: 0x99ffff
		Colour(160, 144, 119),		// BEACH: 0xa09077
		Colour(255, 255, 255),		// SNOW: 0xffffff
		Colour(187, 187, 170),		// TUNDRA: 0xbbbbaa,
		Colour(136, 136, 136),		// BARE: 0x888888,
		Colour(85, 85, 85),			// SCORCHED: 0x555555
		Colour(153, 170, 119),		// TAIGA: 0x99aa77

		Colour(153, 166, 139),
		Colour(228, 232, 202),
		Colour(84, 116, 88),
		Colour(119, 139, 85),
		Colour(153, 180, 112),
		Colour(112, 139, 85),
		Colour(85, 139, 85),
		Colour(172, 159, 139)
	};


void Center::OrderCorners()
{

}

void Center::FixBorders()
{
	for( Corner** c = Corners.begin(); c < Corners.end(); ++c )
	{
		if ((*c)->bBorder)
		{
			(*c)->bOcean = true;
			(*c)->bWater = true;
			bBorder = bOcean = bWater = true;
			break;
		}
	}
	return;
}

// Assign a biome type to each polygon. 
// If it has ocean/coast/water, then that's the biome;
// otherwise it depends on low/high elevation and low/medium/high moisture. 
// This is roughly based on the Whittaker diagram but adapted to fit the
// needs of the island map generator.
void Center::ResetBiome()
{
	if (bOcean) 
	{
		Biome = eBiomeOcean;
    }
	else if (bWater) 
	{
        if (fElevation < 0.1f)
			Biome = eBiomeMarsh;
        else if (fElevation > 0.8f) 
			Biome = eBiomeIce;
		else
			Biome = eBiomeLake;
    }
	else if (bCoast)
	{
        Biome = eBiomeBeach;
    } 
	else if (fElevation > 0.8f)
	{
        if (fMoisture > 0.5f)
			Biome = eBiomeSnow;
        else if (fMoisture > 0.33f)
			Biome = eBiomeTundra;
        else if (fMoisture > 0.16f)
			Biome = eBiomeBare;
        else 
			Biome = eBiomeScorched;
    } 
	else if (fElevation > 0.6f)
	{
        if (fMoisture > 0.66f)
			Biome = eBiomeTaiga;
        else if (fMoisture > 0.33f)
			Biome = eBiomeShrubland;
        else 
			Biome = eBiomeTemperateDesert;
    } 
	else if (fElevation > 0.3f)
	{
        if (fMoisture > 0.83f)
			Biome = eBiomeTemperateRainForest;
        else if (fMoisture > 0.5f) 
			Biome = eBiomeTemperateDeciduousForest;
        else if (fMoisture > 0.16f)
			Biome = eBiomeGrassland;
        else 
			Biome = eBiomeTemperateDesert;
    }
	else
	{
        if (fMoisture > 0.66f)
			Biome = eBiomeTropicalRainForest;
        else if (fMoisture > 0.33f)
			Biome = eBiomeTropicalSeasonalForest;
        else if (fMoisture > 0.16f)
			Biome = eBiomeGrassland;
        else
			Biome = eBiomeSubtropicalDesert;
    }
}

Colour Center::GetBiomeColor(float fElevation, float fMoisture)
{
	if (fElevation > 0.8f)
	{
        if (fMoisture > 0.5f)
			return BiomeColour[eBiomeSnow];
        else if (fMoisture > 0.33f)
			return BiomeColour[eBiomeTundra];
        else if (fMoisture > 0.16f)
			return BiomeColour[eBiomeBare];
        else 
			return BiomeColour[eBiomeScorched];
    } 
	else if (fElevation > 0.6f)
	{
        if (fMoisture > 0.66f)
			return BiomeColour[eBiomeTaiga];
        else if (fMoisture > 0.33f) 
			return BiomeColour[eBiomeShrubland];
        else 
			return BiomeColour[eBiomeTemperateDesert];
    } 
	else if (fElevation > 0.3f) 
	{
        if (fMoisture > 0.83f) 
			return BiomeColour[eBiomeTemperateRainForest];
        else if (fMoisture > 0.5f) 
			return BiomeColour[eBiomeTemperateDeciduousForest];
        else if (fMoisture > 0.16f) 
			return BiomeColour[eBiomeGrassland];
        else 
			return BiomeColour[eBiomeTemperateDesert];
    } 
	else
	{
        if (fMoisture > 0.66f) 
			return BiomeColour[eBiomeTropicalRainForest];
        else if (fMoisture > 0.33f) 
			return BiomeColour[eBiomeTropicalSeasonalForest];
        else if (fMoisture > 0.16f) 
			return BiomeColour[eBiomeGrassland];
        else
			return BiomeColour[eBiomeSubtropicalDesert];
    }
}

String Center::GetBiomeName()
{
	if (bOcean) 
	{
		return String("OCEAN");
    }
	else if (bWater) 
	{
        if (fElevation < 0.1f)
			return String("MARSH");
        else if (fElevation > 0.8f) 
			return String("ICE");
		else
			return String("LAKE");
    }
	else if (bCoast)
	{
        return String("BEACH");
    } 
	else if (fElevation > 0.8f)
	{
        if (fMoisture > 0.5f)
			return String("SNOW");
        else if (fMoisture > 0.33f)
			return String("TUNDRA");
        else if (fMoisture > 0.16f)
			return String("BARE");
        else 
			return String("SCORCHED");
    } 
	else if (fElevation > 0.6f)
	{
        if (fMoisture > 0.66f)
			return String("TAIGA");
        else if (fMoisture > 0.33f) 
			return String("SHRUBLAND");
        else 
			return String("TEMPERATE_DESERT");
    } 
	else if (fElevation > 0.3f) 
	{
        if (fMoisture > 0.83f) 
			return String("TEMPERATE_RAIN_FOREST");
        else if (fMoisture > 0.5f) 
			return String("TEMPERATE_DECIDUOUS_FOREST");
        else if (fMoisture > 0.16f) 
			return String("GRASSLAND");
        else 
			return String("TEMPERATE_DESERT");
    } 
	else
	{
        if (fMoisture > 0.66f) 
			return String("TROPICAL_RAIN_FOREST");
        else if (fMoisture > 0.33f) 
			return String("TROPICAL_SEASONAL_FOREST");
        else if (fMoisture > 0.16f) 
			return String("GRASSLAND");
        else
			return String("SUBTROPICAL_DESERT");
    }
}