#ifndef __MODEL_ROADS_HEADER__
#define __MODEL_ROADS_HEADER__

class CVoionoiMapSetting;
class Edge;
class Center;

class Roads 
{
public:
    // The road array marks the edges that are roads.  The mark is 1, 2, or 3,
	// corresponding to the three contour levels.
	// Note that these are sparse arrays, only filled in where there are roads.
	HashMap<uint32, uint32> road;					// edge index -> int contour level
	HashMap<uint32, Array<Edge*>*>roadConnections;	// center index -> array of Edges with roads


public:
	Roads() { road.clear(); roadConnections.clear(); }
	~Roads() 
	{
		Release();
	}

	void Release();
	void createRoads(const HashMap<uint32, Center*> &CentersMap, CVoionoiMapSetting *pMapSetting);


};

#endif		// __MODEL_ROADS_HEADER__