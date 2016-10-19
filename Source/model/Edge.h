#ifndef __MODEL_EDGE_HEADER__
#define __MODEL_EDGE_HEADER__

class Center;
class Corner;

class Edge
{
public:
	uint32 Index;
	uint32 Key;						// hashed key

	Center* pDelaunayStart;
	Center* pDelaunayEnd;			// Delaunay edge
	Corner* pVoronoiStart;
	Corner* pVoronoiEnd;			// Voronoi edge
	Vector2D Midpoint;				// halfway between v0,v1
	int nRiver;						// volume of water, or 0
	bool bMapEdge;
	Vector2D Point() { return pVoronoiStart->Point; }


public:
    Edge(Corner* begin, Corner* end, Center* left, Center* right)
    {
		Index = 0;
        nRiver = 0;
        pVoronoiStart = begin;
        pVoronoiEnd = end;

        pDelaunayStart = left;
        pDelaunayEnd = right;

		Midpoint.Set((pVoronoiStart->Point.x + pVoronoiEnd->Point.x) / 2, 
					 (pVoronoiStart->Point.y + pVoronoiEnd->Point.y) / 2);
        CreateKey();
    }

    Edge(Corner* begin, Corner* end)
    {
		Index = 0;
        nRiver = 0;
        pVoronoiStart = begin;
        pVoronoiEnd = end;
		pDelaunayStart = NULL;
        pDelaunayEnd = NULL;
		Midpoint.Set(	(pVoronoiStart->Point.x + pVoronoiEnd->Point.x) / 2, 
						(pVoronoiStart->Point.y + pVoronoiEnd->Point.y) / 2);
        Key = Edge::CreateKey();
    }

	bool IsCoast()
    {
        if (pDelaunayStart != NULL && pDelaunayEnd != NULL && pVoronoiStart != NULL && pVoronoiEnd != NULL)
		{
            return ((pVoronoiStart->bCoast) && (pVoronoiEnd->bCoast)
                && !(pDelaunayStart->bWater && pDelaunayEnd->bWater)
                && !(pDelaunayStart->IsLand() && pDelaunayEnd->IsLand()));
		}
        return false;
    }

	Corner* GetCornerStart() { return pVoronoiStart; }
	Corner* GetCornerEnd() { return pVoronoiEnd; }
	float GetDiffX() { return pVoronoiEnd->Point.x - pVoronoiStart->Point.x; }
	float GetDiffY() { return pVoronoiEnd->Point.y - pVoronoiStart->Point.y; }

	bool IsEquals(const Edge& other)
    {
        return pVoronoiStart->IsEquals(*other.pVoronoiStart) && 
            pVoronoiEnd->IsEquals(*other.pVoronoiEnd);
    }
	uint32 CreateKey()
	{
		Vector2D p;
		p.x = (pVoronoiStart->Point.x + pVoronoiEnd->Point.x) / 2;
		p.y = (pVoronoiStart->Point.y + pVoronoiEnd->Point.y) / 2;
		

		Key = ((((uint32)p.x) & 0xFFFF) << 16) | (((uint32)p.y) & 0xFFFF);
		return Key;
	}
};

#endif