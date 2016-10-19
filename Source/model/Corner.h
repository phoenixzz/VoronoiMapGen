#ifndef __MODEL_CORNER_HEADER__
#define __MODEL_CORNER_HEADER__


class Center;
class Edge;

class Corner
{
public:
	uint32 Index;
	uint32 Key;				// hashed key
	Vector2D Point;			// location
	bool bOcean;			// ocean
	bool bWater;			// lake or ocean
	bool IsLand() { return !bWater; }
	void SetLand(bool value) { bWater = !value; }
	bool bCoast;			// touches ocean and land polygons
	bool bBorder;			// at the edge of the map
	float fElevation;		// 0.0 - 1.0
	float fMoisture;		// 0.0 - 1.0

	Array <Center*> Touches;
	Array <Edge*> Protrudes;
	Array <Corner*> Adjacents;

	int32 River;
	Corner* Downslope;
	Corner* Watershed;
	int32 WatershedSize;

public:
    Corner(float ax, float ay)
    {
		Index = 0;
        Point.x = ax;
		Point.y = ay;
        CreateKey();

        bOcean = bWater = bCoast = bBorder = false;
        fMoisture = 0.0f;
        fElevation = 100.0f;

        River = WatershedSize = 0;

		Downslope = NULL;
		Watershed = NULL;
    }

    void AddProtrudes(Edge* edge)
    {
        Protrudes.addIfNotAlreadyThere(edge);
    }

    void AddAdjacent(Corner* corner)
    {
        Adjacents.addIfNotAlreadyThere(corner);
    }

    void AddTouches(Center* center)
    {
		Touches.addIfNotAlreadyThere(center);
    }

	bool IsEquals(const Corner& other)
    {
		return (Point.x == other.Point.x) && (Point.y == other.Point.y);
	}

	uint32 CreateKey()
	{
		Key = ((((uint32)Point.x) & 0xFFFF) << 16) | (((uint32)Point.y) & 0xFFFF);
		return Key;
	}
};


#endif