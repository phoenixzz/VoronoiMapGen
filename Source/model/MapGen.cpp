
#include "../SGPLibraryCode/SGPHeader.h"

#include "../VoronoiMapSetting.h"
#include "../VoronoiImageHelper.h"

#include "../pugixml/pugixml.hpp"

#include "Center.h"
#include "Corner.h"
#include "Edge.h"
#include "NoisyEdges.h"
#include "Roads.h"
#include "PoissonDiskSampling.h"

#include "MapGen.h"

int MapGen::ElevationSorter::compareElements( Corner* first, Corner* second ) noexcept
{
	if( first->fElevation < second->fElevation )
		return -1;
	if( first->fElevation > second->fElevation )
		return 1;
	return 0;
}

int MapGen::MoistureSorter::compareElements( Corner* first, Corner* second ) noexcept
{
	if( first->fMoisture < second->fMoisture )
		return -1;
	if( first->fMoisture > second->fMoisture )
		return 1;
	return 0;
}

MapGen::~MapGen()
{
	ResetMap();
}


Center* MapGen::CreateCenter(float ax, float ay)
{
    Vector2D Point(ax, ay);
    uint32 pKey = ((((uint32)Point.x) & 0xFFFF) << 16) | (((uint32)Point.y) & 0xFFFF);
	if (CentersMap.contains(pKey))
    {
        return CentersMap[pKey];
    }
    else
    {
        Center *nc = new Center(ax, ay);
		nc->Index = CentersMap.size();
		CentersMap.set(nc->Key, nc);		
        return nc;
    }
}

Edge* MapGen::CreateEdge(Corner* begin, Corner* end, Center* Left, Center* Right)
{
    Vector2D p;
	p.x = (begin->Point.x + end->Point.x) / 2;
	p.y = (begin->Point.y + end->Point.y) / 2;

    uint32 pKey = ((((uint32)p.x) & 0xFFFF) << 16) | (((uint32)p.y) & 0xFFFF);
    if (EdgesMap.contains(pKey))
    {
        return EdgesMap[pKey];
    }
    else
    {
        Edge *ne = new Edge(begin, end, Left, Right);
		ne->Index = EdgesMap.size();
        EdgesMap.set(ne->Key, ne);
        return ne;
    }
}

Corner* MapGen::CreateCorner(float ax, float ay)
{
    Vector2D p(ax, ay);
	uint32 pKey = ((((uint32)p.x) & 0xFFFF) << 16) | (((uint32)p.y) & 0xFFFF);

    if (CornersMap.contains(pKey))
    {
        return CornersMap[pKey];
    }
    else
    {
        Corner * nc = new Corner(ax, ay);
		nc->Index = CornersMap.size();
        CornersMap.set(nc->Key, nc);
        return nc;
    }
}


void MapGen::ResetMap()
{
    HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			delete i.getValue();
		}
    }
	CentersMap.clear();

    HashMap<uint32, Edge*>::Iterator j (EdgesMap);
    while (j.next())
    {
        if (j.getValue())
		{
			delete j.getValue();
		}
    }
	EdgesMap.clear();

    HashMap<uint32, Corner*>::Iterator k (CornersMap);
    while (k.next())
    {
        if (k.getValue())
		{
			delete k.getValue();
		}
    }
	CornersMap.clear();

	if (MapNoisyEdges)
	{
		delete MapNoisyEdges;
		MapNoisyEdges = NULL;
	}
	if (MapRoads)
	{
		delete MapRoads;
		MapRoads = NULL;
	}
}

void MapGen::LoadMap(CVoionoiMapSetting* pMapParams)
{
	MapX = pMapParams->uImageWidth;
	MapY = pMapParams->uImageHeight;
	MapSettingParams = pMapParams;

	pMapParams->globalRandom->setSeed(pMapParams->globalSeed);
	pMapParams->islandRandom.setSeed(pMapParams->islandSeed);
	pMapParams->detailRandom.setSeed(pMapParams->detailSeed);

	// Recreate Perlin noise Table
	CreatePerlinTable(pMapParams->islandSeed, 0.01f, 6, 0.6f);

	pMapParams->imagesize = MapX * MapY * 3;
	if (pMapParams->image) 
		free(pMapParams->image);
	pMapParams->image = (unsigned char*)malloc(pMapParams->imagesize);
	memset(pMapParams->image, 0, pMapParams->imagesize);

	uint32 realSitesNum = pMapParams->uSitesNum;

	jcv_point* points = NULL;

	if (pMapParams->RandomGridType == CVoionoiMapSetting::eRelaxed)
	{
		points = (jcv_point*)malloc(sizeof(jcv_point) * realSitesNum);

		int pointoffset = 10; // move the points inwards, for aestetic reasons
		for( int i = 0; i < realSitesNum; ++i )
		{
			points[i].x = (float)(pointoffset + pMapParams->globalRandom->nextInt() % (MapX - 2 * pointoffset));
			points[i].y = (float)(pointoffset + pMapParams->globalRandom->nextInt() % (MapY - 2 * pointoffset));
		}

		for( int i = 0; i < pMapParams->RelaxationNum; ++i )
		{
			jcv_diagram diagram;
			memset(&diagram, 0, sizeof(jcv_diagram));
			jcv_diagram_generate(realSitesNum, (const jcv_point*)points, MapX, MapY, &diagram );

			relax_points(&diagram, points);

			jcv_diagram_free( &diagram );
		}
	}
	else if (pMapParams->RandomGridType == CVoionoiMapSetting::ePoissonDisk)
	{
		double min_dist = pMapParams->uImageWidth / sqrt((double)realSitesNum);
		PoissonDiskSampling PoissonGen((int)MapX, (int)MapY, min_dist, 10, pMapParams->globalSeed);
		Array<Vector2D> PoissonPoint = PoissonGen.Generate();

		realSitesNum = PoissonPoint.size();
		points = (jcv_point*)malloc(sizeof(jcv_point) * realSitesNum);

		for( int i = 0; i < realSitesNum; ++i )
		{
			points[i].x = (float)((int)(PoissonPoint[i].x));
			points[i].y = (float)((int)(PoissonPoint[i].y));
		}
	}
	else if (pMapParams->RandomGridType == CVoionoiMapSetting::eSquare)
	{
		realSitesNum = (uint32)(sqrt((double)pMapParams->uSitesNum));
		points = (jcv_point*)malloc(sizeof(jcv_point) * realSitesNum * realSitesNum);

		uint32 idx = 0;
		for (uint32 r = 0; r < realSitesNum; r++)
		{
			for (int32 l = 0; l < realSitesNum ; l++) 
			{
				points[idx].x = (float)((int)((0.5f + r) / realSitesNum * MapX));
				points[idx].y = (float)((int)((0.5f + l) / realSitesNum * MapY));
				idx++;
			}
		}
		realSitesNum = realSitesNum * realSitesNum;
	}
	else if (pMapParams->RandomGridType == CVoionoiMapSetting::eHexagon)
	{
		realSitesNum = (uint32)(sqrt((double)pMapParams->uSitesNum));
		points = (jcv_point*)malloc(sizeof(jcv_point) * realSitesNum * realSitesNum);
		uint32 idx = 0;
		for (uint32 r = 0; r < realSitesNum; r++)
		{
			for (uint32 l = 0; l < realSitesNum; l++) 
			{
				points[idx].x = (float)((int)((0.5f + r) / realSitesNum * MapX));
				points[idx].y = (float)((int)((0.25f + 0.5f * (r % 2) + l) / realSitesNum * MapY));
				idx++;
			}
		}
		realSitesNum = realSitesNum * realSitesNum;	
	}

	jcv_diagram diagram;
	memset(&diagram, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(realSitesNum, (const jcv_point*)points, MapX, MapY, &diagram );

	ImproveMapData(diagram);

	jcv_diagram_free( &diagram );

	free(points);
	points = NULL;

	CreateIsland();

	CreateRoads();
	
	BuildNoisyEdges();
	
	DrawMap();

	return;
}

void MapGen::ImproveMapData(jcv_diagram& diagram)
{
	// The Map gen FLOAT epsilon
	float epsilon_map = 0.0001f;

	// iterate over the sites and get all edges
	const jcv_site* sites = jcv_diagram_get_sites( &diagram );
	for( int i = 0; i < diagram.numsites; ++i )
	{
		const jcv_site* site = &sites[i];

		Center* curCenter = CreateCenter(site->p.x, site->p.y);

		jcv_graphedge* ge = site->edges;
		while( ge )
		{
			if (jcv_point_dist_sq(&(ge->pos[0]), &(ge->pos[1])) < epsilon_map)
			{
				ge = ge->next;
				continue;
			}

			jcv_edge* edge = ge->edge;
			if (FixPoints(*ge))
			{
				Corner* c1 = CreateCorner(ge->pos[0].x + epsilon_map, ge->pos[0].y + epsilon_map);
				Corner* c2 = CreateCorner(ge->pos[1].x + epsilon_map, ge->pos[1].y + epsilon_map);
				Center* cntrLeft = CreateCenter(edge->sites[0]->p.x, edge->sites[0]->p.y);
				Center* cntrRight = CreateCenter(edge->sites[1]->p.x, edge->sites[1]->p.y);
				
				c1->bBorder = ((uint32)c1->Point.x == 0 || (uint32)c1->Point.x == MapX - 1 || (uint32)c1->Point.y == 0 || (uint32)c1->Point.y == MapY - 1);
				c2->bBorder = ((uint32)c2->Point.x == 0 || (uint32)c2->Point.x == MapX - 1 || (uint32)c2->Point.y == 0 || (uint32)c2->Point.y == MapY - 1);

				c1->AddAdjacent(c2);
				c2->AddAdjacent(c1);

				curCenter->Corners.addIfNotAlreadyThere(c1);
				curCenter->Corners.addIfNotAlreadyThere(c2);

				Edge* e = CreateEdge(c1, c2, cntrLeft, cntrRight);

				cntrLeft->Borders.addIfNotAlreadyThere(e);
				cntrRight->Borders.addIfNotAlreadyThere(e);

				cntrLeft->Neighbours.addIfNotAlreadyThere(cntrRight);
				cntrRight->Neighbours.addIfNotAlreadyThere(cntrLeft);

				c1->AddProtrudes(e);
				c2->AddProtrudes(e);
				c1->AddTouches(cntrLeft);
				c1->AddTouches(cntrRight);
				c2->AddTouches(cntrLeft);
				c2->AddTouches(cntrRight);
			}

			ge = ge->next;
		}
	}

	HashMap<uint32, Corner*>::Iterator qm (CornersMap);
    while (qm.next())
    {
		if (qm.getValue())
		{
			Corner* q = qm.getValue();
            if (!q->bBorder)
            {
                Vector2D point(0, 0);
				for( Center** c = q->Touches.begin(); c < q->Touches.end(); ++c )
				{
                    point.x += (*c)->Point.x;
                    point.y += (*c)->Point.y;
                }
                point.x = point.x / q->Touches.size();
                point.y = point.y / q->Touches.size();
                q->Point.x = point.x;
				q->Point.y = point.y;
            }
        }
	}

    // The edge midpoints were computed for the old corners and need to be recomputed.
	HashMap<uint32, Edge*>::Iterator em (EdgesMap);
	while (em.next())
    {
		Edge* e = em.getValue();
		if (e && e->pVoronoiStart && e->pVoronoiEnd)
		{
			e->Midpoint.Set(
				(e->pVoronoiStart->Point.x + e->pVoronoiEnd->Point.x) / 2,
				(e->pVoronoiStart->Point.y + e->pVoronoiEnd->Point.y) / 2);
        }
	}	
}

bool MapGen::FixPoints(jcv_graphedge& edge)
{
	float x1 = edge.pos[0].x;
	float y1 = edge.pos[0].y;
	float x2 = edge.pos[1].x;
	float y2 = edge.pos[1].y;

    //if both ends are in map, not much to do
    if ((DotInMap(x1, y1) && DotInMap(x2, y2)))
        return true;
            
    //if one end is out of map
    if ((DotInMap(x1, y1) && !DotInMap(x2, y2)) || (!DotInMap(x1, y1) && DotInMap(x2, y2)))
    {
        float b = 0.0f, slope = 0.0f;

		if (x2 == x1)
		{
			if (edge.pos[0].x < 0)
				edge.pos[0].x = 0;
			if (edge.pos[0].x >= MapX)
				edge.pos[0].x = MapX - 1;
			if (edge.pos[0].y < 0)
				edge.pos[0].y = 0;		
			if (edge.pos[0].y >= MapY)
				edge.pos[0].y = MapY - 1;
	        if (edge.pos[1].x < 0)
				edge.pos[1].x = 0;
			if (edge.pos[1].x >= MapX)
				edge.pos[1].x = MapX - 1;
			if (edge.pos[1].y < 0)
				edge.pos[1].y = 0;
			if (edge.pos[1].y >= MapY)
				edge.pos[1].y = MapY - 1;		

			return true;
		}

        slope = ((y2 - y1) / (x2 - x1));

        b = edge.pos[0].y - (slope * edge.pos[0].x);

        // y = ( slope * x ) + b

        if (edge.pos[0].x < 0)
		{
            edge.pos[0].x = 0;
			edge.pos[0].y = b;
		}
        if (edge.pos[0].x >= MapX)
		{
			edge.pos[0].x = MapX - 1;
            edge.pos[0].y = (MapX - 1) * slope + b;
		}
        if (edge.pos[0].y < 0)
		{
            edge.pos[0].x = -b / slope;
			edge.pos[0].y = 0;
		}
        if (edge.pos[0].y >= MapY)
		{
            edge.pos[0].x = (MapY - 1 - b) / slope;
			edge.pos[0].y = MapY - 1;
		}



        if (edge.pos[1].x < 0)
		{
			edge.pos[1].x = 0;
			edge.pos[1].y = b;
   		}
        if (edge.pos[1].x >= MapX)
		{
			edge.pos[1].x = MapX - 1;
            edge.pos[1].y = (MapX - 1) * slope + b;
		}
        if (edge.pos[1].y < 0)
		{
            edge.pos[1].x = -b / slope;
			edge.pos[1].y = 0;
		}
        if (edge.pos[1].y >= MapY)
		{
            edge.pos[1].x = (MapY - 1 - b) / slope;
			edge.pos[1].y = MapY - 1;
		}

		return true;
	}
	else
	{
		edge.pos[0].x = jmin(jmax(edge.pos[0].x, 0.0f), (float)(MapX - 1));
		edge.pos[0].y = jmin(jmax(edge.pos[0].y, 0.0f), (float)(MapY - 1));
		edge.pos[1].x = jmin(jmax(edge.pos[1].x, 0.0f), (float)(MapX - 1));
		edge.pos[1].y = jmin(jmax(edge.pos[1].y, 0.0f), (float)(MapY - 1));

		return true;
	}
}

void MapGen::CreateIsland()
{
	HashMap<uint32, Corner*>::Iterator cm (CornersMap);
	while (cm.next())
    {
		if(cm.getValue())
        {
            cm.getValue()->bWater = !InLand(cm.getValue()->Point); // calculate land&water corners
        }
	}
    
	FixCentersFloodFillOceans();

	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* c = cc.getValue();
			bool bWater = false;
			bool bOcean = true;
			bool bLand = false;
			for( Center** ce = c->Touches.begin(); ce < c->Touches.end(); ++ce )
			{
				bWater = bWater || (*ce)->bWater;
				bOcean = bOcean && (*ce)->bOcean;
				bLand = bLand || (*ce)->IsLand();
			}
			c->bCoast = bWater && bLand;
			c->bWater = !bLand;
			c->bOcean = bOcean;			
		}
    }

	// Determine the elevations
	CalculateElevation();
    // Rescale elevations so that the highest is 1.0, and they're
    // distributed well. We want lower elevations to be more common
    // than higher elevations, in proportions approximately matching
    // concentric rings. That is, the lowest elevation is the
    // largest ring around the island, and therefore should more
    // land area than the highest elevation, which is the very
    // center of a perfectly circular island.
	RedistributeElevation();

	// Determine downslope paths.
	CalculateDownslopes();
	// Determine watersheds: for every corner, where does it flow out into the ocean? 
	CalculateWatersheds();
	// Create rivers.
	CreateRivers();
	// Determine moisture at corners, starting at rivers and lakes, but not oceans. 
	// Then redistribute moisture to cover the entire range evenly from 0.0 to 1.0. 
	// Then assign polygon moisture as the average of the corner moisture.
	CalculateCornerMoisture();


    // Assign a biome type to each polygon.
    HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			i.getValue()->ResetBiome();
		}
	}
}

bool MapGen::InLand(Vector2D p)
{
	if (MapSettingParams->IslandShapeType == CVoionoiMapSetting::eRadialIsland)
	{
		return IsLandShapeRadial(Vector2D(2 * (p.x / MapX - 0.5f), 2 * (p.y / MapY - 0.5f)));
	}
	else if (MapSettingParams->IslandShapeType == CVoionoiMapSetting::ePerlinIsland)
	{
		return IsLandShapePerlin(Vector2D(2 * (p.x / MapX - 0.5f), 2 * (p.y / MapY - 0.5f)));
	}
	else if (MapSettingParams->IslandShapeType == CVoionoiMapSetting::eSquareIsland)
	{
		return IsLandShapeSquare(Vector2D(2 * (p.x / MapX - 0.5f), 2 * (p.y / MapY - 0.5f)));
	}
	else if (MapSettingParams->IslandShapeType == CVoionoiMapSetting::eBlobIsland)
	{
		return IsLandShapeBlob(Vector2D(2 * (p.x / MapX - 0.5f), 2 * (p.y / MapY - 0.5f)));
	}


	return false;
}

// Function takes a normalized point (x and y are -1 to +1) and returns true if the
// point should be on the island, and false if it should be water (lake or ocean).
bool MapGen::IsLandShapeRadial(Vector2D point)
{
	// 1.0 means no small islands; 2.0 leads to a lot
    const float ISLAND_FACTOR = 1.07f;

	int bumps = MapSettingParams->islandRandom.nextInt(6) + 1;
	double startAngle = MapSettingParams->islandRandom.nextDouble() * double_2pi;
    double dipAngle = MapSettingParams->islandRandom.nextDouble() * double_2pi;
    double dipWidth = (MapSettingParams->islandRandom.nextInt(6) + 2) / 10;

    double angle = atan2f(point.y, point.x);
	double length = 0.5 * (jmax(abs(point.x), abs(point.y)) + point.GetLength());

    double r1 = 0.5 + 0.40 * sinf(startAngle + bumps * angle + cosf((bumps + 3) * angle));
    double r2 = 0.7 - 0.20 * sinf(startAngle + bumps * angle - sinf((bumps + 2) * angle));
    if (abs(angle - dipAngle) < dipWidth || abs(angle - dipAngle + double_2pi) < dipWidth ||
		abs(angle - dipAngle - double_2pi) < dipWidth)
    {
        r1 = r2 = 0.2;
    }
    return (length < r1 || (length > r1 * ISLAND_FACTOR && length < r2));
}

bool MapGen::IsLandShapePerlin(Vector2D point)
{
	float c = PerlinNoiseTable[int((point.x + 1) * 128)][int((point.y + 1) * 128)];
	return c > (0.3f + 0.3f * point.GetLength() * point.GetLength());
}

bool MapGen::IsLandShapeSquare(Vector2D point)
{
	return true;
}
bool MapGen::IsLandShapeBlob(Vector2D p)
{
	bool eye1 = Vector2D(p.x-0.2f, p.y*0.5f+0.2f).GetLength() < 0.05f;
	bool eye2 = Vector2D(p.x+0.2f, p.y*0.5f+0.2f).GetLength() < 0.05f;
	bool body = p.GetLength() < 0.8f - 0.18f * sin(5.0f * atan2(p.y, p.x));
	return body && !eye1 && !eye2;
}

void MapGen::CreatePerlinTable(int64 seed, float scale, int32 octaves, float falloff)
{
	CPerlinNoise perlin(seed);
	for( int height=0; height < 256; height++ )
	{
		for( int width=0; width < 256; width++ )
		{
			float accum = 0;
			float frequency = scale;
			float amplitude = 1.0f;

			for(int32 i=0; i<octaves; ++i)
			{
				accum += perlin.noise(width, height, frequency) * amplitude;
				amplitude *= falloff;
				frequency *= 2.0f;
			}

			accum = jlimit( -1.0f, 1.0f, accum );
			accum *= 0.5f;
			accum += 0.5f;
			PerlinNoiseTable[height][width] = accum;
		}
	}
}

void MapGen::FixCentersFloodFillOceans()
{
    HashMap<uint32, Center*>::Iterator ct (CentersMap);
    while (ct.next())
    {
        if (ct.getValue())
		{
            ct.getValue()->FixBorders(); //Fix edges at map border , set "border" and "ocean" values
            ct.getValue()->OrderCorners(); //Order corners clockwise as we'Ll need it for polygons and 3d stuff

            //if it touches any water corner , it's water ; there will be leftovers tho
			bool bWater = false;
			for( Corner** c = ct.getValue()->Corners.begin(); c < ct.getValue()->Corners.end(); ++c )
			{
				bWater = bWater || (*c)->bWater;
			}
            ct.getValue()->bWater = bWater;			
		}
    }

	RingFIFO<Center*> tempOceans(findMinimumPower2Number(CentersMap.size()));
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		//start with oceans at the borders
        if (i.getValue())
		{
			Center* c = i.getValue();
			if (c->bOcean)
				tempOceans.push(&c, 1);
		}
	}

    //floodfill oceans
    while (tempOceans.getSize() > 0)
    {
        Center* c = NULL;
		tempOceans.pop(&c, 1);

		for( Center** n = c->Neighbours.begin(); n < c->Neighbours.end(); ++n )
		{
			if (!(*n)->bOcean)
			{
				bool bOcean = false;
				for( Corner** ss = (*n)->Corners.begin(); ss < (*n)->Corners.end(); ++ss )
				{
					bOcean = bOcean || (*ss)->bWater;
				}
				if (bOcean)
				{
					(*n)->bOcean = true;
					if (!tempOceans.contains(*n))
					{
						tempOceans.push(n, 1);
					}
				}
				else
				{
					(*n)->bCoast = true;
				}
			}
		}
    }
}

void MapGen::CalculateElevation()
{
	RingFIFO<Corner*> tempCorners(findMinimumPower2Number(CornersMap.size()));

	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* q = cc.getValue();
			// The edges of the map are elevation 0
			if (q->bBorder) 
			{
				q->fElevation = 0;
				tempCorners.push(&q, 1);
			} 
			else 
			{
				q->fElevation = DBL_MAX;
			}
		}
	}

	// Traverse the graph and assign elevations to each point. As we
    // move away from the map border, increase the elevations. This
    // guarantees that rivers always have a way down to the coast by
    // going downhill (no local minima).
	while (tempCorners.getSize() > 0) 
    {
		Corner* q = NULL;
		tempCorners.pop(&q, 1);

		for( Corner** adj = q->Adjacents.begin(); adj < q->Adjacents.end(); ++adj )
		{
			// Every step up is epsilon over water or 1 over land. The
            // number doesn't matter because we'll rescale the
            // elevations later.
            double newElevation = 0.01 + q->fElevation;                    
            if (!q->bWater && !(*adj)->bWater) 
            {
                newElevation += 1.0;
				if (MapSettingParams->NeedMoreRandom())
				{
					// HACK: the map looks nice because of randomness of points,
					// randomness of rivers, and randomness of edges.
					// Without random point selection, I needed to inject some more randomness to 
					// make maps look nicer. I'm doing it here, with elevations,
					// but I think there must be a better way. 
					// This hack is only used with square/hexagon grids.
					newElevation += MapSettingParams->detailRandom.nextDouble();
				}
            }

            // If this point changed, we'll add it to the queue so that we can process its neighbors too.
            if (newElevation < (*adj)->fElevation) 
            {
                (*adj)->fElevation = newElevation;
                tempCorners.push(adj, 1);
            }
        }
    }
}

// Change the overall distribution of elevations so that lower elevations are more common than higher elevations.
// Specifically, we want elevation X to have frequency (1-X).  To do this we will sort the corners, 
// then set each corner to its desired elevation.
void MapGen::RedistributeElevation()
{
    // SCALE_FACTOR increases the mountain area. At 1.0 the maximum
    // elevation barely shows up on the map, so we set it to 1.1.
    double scaleFactor = 1.1;
	Array<Corner*> locations;

	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* q = cc.getValue();
			if (!q->bOcean && !q->bCoast)
				locations.add(q);
		}
	}

	ElevationSorter CompareElevationInstance;
	locations.sort(CompareElevationInstance);

	for (int i = 0; i < locations.size(); i++)
    {
        double y = (double)i / (locations.size() - 1);

        double x = 1.04880885 - sqrt(scaleFactor * (1 - y));
        if (x > 1.0) 
            x = 1.0;  
		locations.getReference(i)->fElevation = x;
    }
	locations.clear();



    // Assign elevations to non-land corners
	HashMap<uint32, Corner*>::Iterator ccc (CornersMap);
    while (ccc.next())
    {
		if(ccc.getValue())
        {
			Corner* q = ccc.getValue();
			if (q->bOcean || q->bCoast)
			{
				q->fElevation = 0;
			}
        }
	}
	// Polygon elevations are the average of their corners
	double sumElevation = 0;
    HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			Center* p = i.getValue();
			sumElevation = 0.0;
			for (int j=0; j < p->Corners.size(); j++) 
			{
				sumElevation += p->Corners.getReference(j)->fElevation;
            }
			p->fElevation = sumElevation / p->Corners.size();
        }
	}
}

// Calculate downslope pointers.  At every point, we point to the
// point downstream from it, or to itself.  
// This is used for generating rivers and watersheds.
void MapGen::CalculateDownslopes()
{
	HashMap<uint32, Corner*>::Iterator ccc (CornersMap);
    while (ccc.next())
    {
		if(ccc.getValue())
        {
			Corner* corner = ccc.getValue();
			Corner* buf = corner;

			for(int i=0; i<corner->Adjacents.size(); i++)
			{
				if(corner->Adjacents[i]->fElevation < buf->fElevation)
					buf = corner->Adjacents[i];
			}

			corner->Downslope = buf;
		}
	}
}

// Calculate the watershed of every land point. The watershed is
// the last downstream land point in the downslope graph.
void MapGen::CalculateWatersheds()
{
	// Initially the watershed pointer points downslope one step. 
	HashMap<uint32, Corner*>::Iterator ccc (CornersMap);
    while (ccc.next())
    {
		if(ccc.getValue())
        {
			Corner* q = ccc.getValue();
            q->Watershed = q;                
            if (!q->bOcean && !q->bCoast) 
            {
                q->Watershed = q->Downslope;
            }
        }
	}
    // Follow the downslope pointers to the coast. Limit to 100
    // iterations although most of the time with numPoints==2000 it
    // only takes 20 iterations because most points are not far from
    // a coast.
	for (int i = 0; i < 100; i++) 
    {
        bool changed = false;

		HashMap<uint32, Corner*>::Iterator ccc (CornersMap);
		while (ccc.next())
		{
			if(ccc.getValue())
			{
				Corner* q = ccc.getValue();
                if (!q->bOcean && !q->bCoast && !q->Watershed->bCoast) 
                {
                    Corner* r = q->Downslope->Watershed;
                        
                    if (!r->bOcean) 
					{
                        q->Watershed = r;                        
						changed = true;
					}
				}
            }
		}
        if (!changed) 
            break;
    }
	// How big is each watershed?
	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* r = cc.getValue()->Watershed;
            r->WatershedSize = 1 + r->WatershedSize;
        }
	}
}

// Create rivers along edges. Pick a random corner point, then
// move downslope. Mark the edges and corners as rivers.
void MapGen::CreateRivers()
{
    for (int i = 0; i < MapX / 2; i++)
    {
		HashMap<uint32, Corner*>::Iterator cc (CornersMap);
		int32 num = MapSettingParams->detailRandom.nextInt(CornersMap.size());		
		while (num >= 0)
        {
			if (cc.next())
			{
				num--;
			}
		}
		Corner *q = cc.getValue();
                
        if (q->bOcean || q->fElevation < 0.3 || q->fElevation > 0.9) 
			continue;
                
        while (!q->bCoast)
        {
            if (q == q->Downslope)
            {
                break;
            }
			for (Edge** ed = q->Protrudes.begin(); ed < q->Protrudes.end(); ++ed)
			{ 
				if ((*ed)->pVoronoiStart == q->Downslope || (*ed)->pVoronoiEnd == q->Downslope)
				{
					(*ed)->nRiver = (*ed)->nRiver + 1;
					q->River = q->River + 1;
					q->Downslope->River = q->Downslope->River + 1; 
					q = q->Downslope;
					break;
				}
			}
        }
    }
}

// Calculate moisture. Freshwater sources spread moisture: rivers and lakes (not oceans).
// Saltwater sources have moisture but do not spread it (we set it at the end, after propagation).
void MapGen::CalculateCornerMoisture()
{
	RingFIFO<Corner*> queue;

	// Fresh water
	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* q = cc.getValue();

			if ((q->bWater || q->River > 0) && !q->bOcean) 
			{
				q->fMoisture = q->River > 0 ? jmin(3.0f, (0.2f * q->River)) : 1.0f;
				queue.push(&q, 1);
			} 
			else 
			{
				q->fMoisture = 0.0f;
			}
		}
    }
	while (queue.getSize() > 0) 
    {
		Corner* q = NULL;
        queue.pop(&q, 1);

		for (Corner** r = q->Adjacents.begin(); r < q->Adjacents.end(); ++r)
		{
			float newMoisture = q->fMoisture * 0.9f;
            if (newMoisture > (*r)->fMoisture) 
            {
                (*r)->fMoisture = newMoisture;
                queue.push(r, 1);
            }
        }
    }
	// Salt water
	HashMap<uint32, Corner*>::Iterator ccc (CornersMap);
	while (ccc.next())
    {
		if(ccc.getValue())
        {
			if (ccc.getValue()->bOcean || ccc.getValue()->bCoast) 
			{
				ccc.getValue()->fMoisture = 1.0f;
			}
		}
    }

	// Change the overall distribution of moisture to be evenly distributed.
	Array<Corner*> locations;
	HashMap<uint32, Corner*>::Iterator cccc (CornersMap);
    while (cccc.next())
    {
		if(cccc.getValue())
        {
			Corner* q = cccc.getValue();
			if (!q->bOcean && !q->bCoast)
				locations.add(q);
		}
	}

	MoistureSorter CompareMoistureInstance;
	locations.sort(CompareMoistureInstance);

	for (int i = 0; i < locations.size(); i++) 
    {
		locations.getReference(i)->fMoisture = (float) i / (locations.size() - 1);
    }

	// Polygon moisture is the average of the moisture at corners
	double sumMoisture = 0;
    HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			Center* p = i.getValue();
			sumMoisture = 0.0;
			for (int j=0; j < p->Corners.size(); j++) 
			{
				if (p->Corners.getReference(j)->fMoisture > 1.0f)
					p->Corners.getReference(j)->fMoisture = 1.0f;
				sumMoisture += p->Corners.getReference(j)->fMoisture;
            }
			p->fMoisture = sumMoisture / p->Corners.size();
        }
	}
}

void MapGen::BuildNoisyEdges()
{
	if (!MapNoisyEdges)
	{
		MapNoisyEdges = new NoisyEdges();		
	}
	MapNoisyEdges->BuildNoisyEdges(CentersMap, MapSettingParams);
}

void MapGen::CreateRoads()
{
	if (!MapRoads)
	{
		MapRoads = new Roads();
		MapRoads->createRoads(CentersMap, MapSettingParams);
	}
}

void MapGen::Fill3DVertex(Array<Array<SGPVertex_UPOS_VERTEXCOLOR>> &VertexList, Array<Array<uint16>> &VertexIdxList)
{
	VertexList.clear();
	VertexIdxList.clear();

	Array<SGPVertex_UPOS_VERTEXCOLOR> vertex_list;
	Array<uint16> index_list;

	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;
			Colour col = Center::BiomeColour[c->Biome];

			if (c->bOcean)
			{
				for( Corner** ee = c->Corners.begin(); ee < c->Corners.end(); ++ee )
				{
					int idx = vertex_list.size();
					SGPVertex_UPOS_VERTEXCOLOR v0, v1, v2;
					v0.x = p.x; v0.y = c->fElevation * 0.15f * MapSettingParams->uImageWidth; v0.z = p.y;

					jcv_point e0, e1;
					e0.x = (*ee)->Point.x;
					e0.y = (*ee)->Point.y;
					v1.x = e0.x; v1.y = (*ee)->fElevation * 0.15f * MapSettingParams->uImageWidth; v1.z = e0.y;

					if (ee + 1 < c->Corners.end())
					{
						e1.x = (*(ee+1))->Point.x;
						e1.y = (*(ee+1))->Point.y;
						v2.x = e1.x; v2.y = (*(ee+1))->fElevation * 0.15f * MapSettingParams->uImageWidth; v2.z = e1.y;
					}
					else
					{
						e1.x = c->Corners.getFirst()->Point.x;
						e1.y = c->Corners.getFirst()->Point.y;
						v2.x = e1.x; v2.y = c->Corners.getFirst()->fElevation * 0.15f * MapSettingParams->uImageWidth; v2.z = e1.y;
					}

				
					v0.VertexColor[0] = v1.VertexColor[0] = v2.VertexColor[0] = col.getFloatRed();
					v0.VertexColor[1] = v1.VertexColor[1] = v2.VertexColor[1] = col.getFloatGreen();
					v0.VertexColor[2] = v1.VertexColor[2] = v2.VertexColor[2] = col.getFloatBlue();
					v0.VertexColor[3] = v1.VertexColor[3] = v2.VertexColor[3] = 1.0f;
					vertex_list.add(v0); vertex_list.add(v1); vertex_list.add(v2);				
					index_list.add(idx); index_list.add(idx+1); index_list.add(idx+2); 
				}
			}
			else
			{
				for(int idx = 0; idx < c->Corners.size(); ++idx )
				{
					Corner* corner0 = c->Corners[idx];
					Corner* corner1 = (idx+1 >= c->Corners.size()) ? c->Corners[0] : c->Corners[idx+1];

					int idd = vertex_list.size();
					SGPVertex_UPOS_VERTEXCOLOR v0, v1, v2;
					v0.x = p.x; v0.y = c->fElevation * 0.15f * MapSettingParams->uImageWidth; v0.z = p.y;


					for(Edge** edge = c->Corners[idx]->Protrudes.begin(); edge < c->Corners[idx]->Protrudes.end(); ++edge)
					{
						if ((*edge)->GetCornerStart() == corner0 && (*edge)->GetCornerEnd() == corner1)
						{						
							Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
								getSlopeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, *edge) :
								getSlopeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, *edge);

							v1.x = corner0->Point.x; v1.y = corner0->fElevation * 0.15f * MapSettingParams->uImageWidth; v1.z = corner0->Point.y;
							v2.x = corner1->Point.x; v2.y = corner1->fElevation * 0.15f * MapSettingParams->uImageWidth; v2.z = corner1->Point.y;
							v0.VertexColor[0] = v1.VertexColor[0] = v2.VertexColor[0] = blendcol.getFloatRed();
							v0.VertexColor[1] = v1.VertexColor[1] = v2.VertexColor[1] = blendcol.getFloatGreen();
							v0.VertexColor[2] = v1.VertexColor[2] = v2.VertexColor[2] = blendcol.getFloatBlue();
							v0.VertexColor[3] = v1.VertexColor[3] = v2.VertexColor[3] = 1.0f;
							vertex_list.add(v0); vertex_list.add(v1); vertex_list.add(v2);
							index_list.add(idd); index_list.add(idd+1); index_list.add(idd+2); 

							break;						
						}
						else if ((*edge)->GetCornerStart() == corner1 && (*edge)->GetCornerEnd() == corner0)
						{
							Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
								getSlopeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, *edge) :
								getSlopeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, *edge);

							v1.x = corner1->Point.x; v1.y = corner1->fElevation * 0.15f * MapSettingParams->uImageWidth; v1.z = corner1->Point.y;
							v2.x = corner0->Point.x; v2.y = corner0->fElevation * 0.15f * MapSettingParams->uImageWidth; v2.z = corner0->Point.y;

							v0.VertexColor[0] = v1.VertexColor[0] = v2.VertexColor[0] = blendcol.getFloatRed();
							v0.VertexColor[1] = v1.VertexColor[1] = v2.VertexColor[1] = blendcol.getFloatGreen();
							v0.VertexColor[2] = v1.VertexColor[2] = v2.VertexColor[2] = blendcol.getFloatBlue();
							v0.VertexColor[3] = v1.VertexColor[3] = v2.VertexColor[3] = 1.0f;
							vertex_list.add(v0); vertex_list.add(v2); vertex_list.add(v1);
							index_list.add(idd); index_list.add(idd+1); index_list.add(idd+2);

							break;
						}
					}
				}
			}


			if (vertex_list.size() > 4096)
			{
				VertexList.add(vertex_list);
				VertexIdxList.add(index_list);
				vertex_list.clear();
				index_list.clear();
			}
		}
	}

	if (vertex_list.size() > 0 || index_list.size() > 0)
	{
		VertexList.add(vertex_list);
		VertexIdxList.add(index_list);
		vertex_list.clear();
		index_list.clear();
	}
}

void MapGen::DrawMap()
{
	if (MapSettingParams->image)
	{
		memset(MapSettingParams->image, 0, MapSettingParams->imagesize);

		//  just fill everything with ocean first.
		Colour col = Center::BiomeColour[Center::eBiomeOcean];
		unsigned char color_tri[3];
		color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();
		jcv_point p0, p1, p2, p3;
		p0.x = 0; p0.y = 0;
		p1.x = 0; p1.y = MapY - 1;
		p2.x = MapX - 1; p2.y = MapY - 1;
		p3.x = MapX - 1; p3.y = 0;
		draw_triangle( &p0, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
		draw_triangle( &p0, &p3, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);

		if (MapSettingParams->RenderMode == CVoionoiMapSetting::e3DSlopes)
		{
		}
		else
		{
			if (MapSettingParams->RenderMode == CVoionoiMapSetting::ePolygons)
			{
				renderPolygons();
			}
			else if (MapSettingParams->RenderMode == CVoionoiMapSetting::e2DSlopes)
			{
				renderSlopePolygons();
			}
			else if (MapSettingParams->RenderMode == CVoionoiMapSetting::eSmoothPolygons)
			{
				renderSmoothPolygons();				
			}
			else if (MapSettingParams->RenderMode == CVoionoiMapSetting::eBiome)
			{
				renderNoisyPolygons();
			}

			if (MapSettingParams->RenderMode != CVoionoiMapSetting::e2DSlopes &&
				MapSettingParams->RenderMode != CVoionoiMapSetting::e3DSlopes &&
				MapSettingParams->NeedRoad)
			{
				renderRoads();
			}

			renderEdges();
		}
	}

}

void MapGen::renderPolygons()
{
	unsigned char color_point[3] = {255, 255, 255};
	unsigned char color_tri[3] = { 0, 0, 0 };

	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			jcv_point p;
			p.x = c->Point.x;
			p.y = c->Point.y;

			for( Corner** ee = c->Corners.begin(); ee < c->Corners.end(); ++ee )
			{
				jcv_point e0, e1;
				e0.x = (*ee)->Point.x;
				e0.y = (*ee)->Point.y;
				if (ee + 1 < c->Corners.end())
				{
					e1.x = (*(ee+1))->Point.x;
					e1.y = (*(ee+1))->Point.y;
				}
				else
				{
					e1.x = c->Corners.getFirst()->Point.x;
					e1.y = c->Corners.getFirst()->Point.y;
				}

				Colour col = Center::BiomeColour[c->Biome];
				color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();
				draw_triangle( &p, &e0, &e1, MapSettingParams->image, MapX, MapY, 3, color_tri);				

			}

			// Plot the sites
			plot((int)p.x, (int)p.y, MapSettingParams->image, MapX, MapY, 3, color_point);
		}
	}
}

Colour MapGen::interpolateEdgeColor(Colour inColor, Center* p, Center* q, float f)
{		
	if (q != NULL && p != NULL && p->bWater == q->bWater)
	{
		Colour col_p = Center::BiomeColour[p->Biome];
		Colour col_q = Center::BiomeColour[q->Biome];
		inColor = Colour::fromFloatRGBA(
			col_p.getFloatRed() * (1.0f - f) + col_q.getFloatRed() * f,
			col_p.getFloatGreen() * (1.0f - f) + col_q.getFloatGreen() * f,
			col_p.getFloatBlue() * (1.0f - f) + col_q.getFloatBlue() * f,
			1.0f);
	}
	return inColor;
}

void MapGen::renderSmoothPolygons()
{
	unsigned char color_tri[3] = {255, 255, 255};

	// First Fill Ocean Polygons
	HashMap<uint32, Center*>::Iterator m (CentersMap);
    while (m.next())
    {
		Center* c = m.getValue();
		if (c && c->bOcean)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;
			for( Corner** ee = c->Corners.begin(); ee < c->Corners.end(); ++ee )
			{
				jcv_point e0, e1;
				e0.x = (*ee)->Point.x;
				e0.y = (*ee)->Point.y;
				if (ee + 1 < c->Corners.end())
				{
					e1.x = (*(ee+1))->Point.x;
					e1.y = (*(ee+1))->Point.y;
				}
				else
				{
					e1.x = c->Corners.getFirst()->Point.x;
					e1.y = c->Corners.getFirst()->Point.y;
				}

				draw_triangle( &p, &e0, &e1, MapSettingParams->image, MapX, MapY, 3, color_tri);
			}
		}
	}

	// Second Fill other Biome Polygons
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;

			if (c->bOcean)
			{
				continue;
			}
			
			for(int idx = 0; idx < c->Corners.size(); ++idx )
			{
				Corner* corner0 = c->Corners[idx];
				Corner* corner1 = (idx+1 >= c->Corners.size()) ? c->Corners[0] : c->Corners[idx+1];

				for(Edge** edge = c->Corners[idx]->Protrudes.begin(); edge < c->Corners[idx]->Protrudes.end(); ++edge)
				{
					if (!MapNoisyEdges->path0.contains((*edge)->Key) || !MapNoisyEdges->path1.contains((*edge)->Key))
						continue;

					Array<Vector2D>* path0 = MapNoisyEdges->path0[(*edge)->Key];
					Array<Vector2D>* path1 = MapNoisyEdges->path1[(*edge)->Key];

					if ((*edge)->GetCornerStart() == corner0 && (*edge)->GetCornerEnd() == corner1)
					{
						
						Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
							interpolateEdgeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, 0.3f) :
							interpolateEdgeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, 0.3f);
						color_tri[0] = blendcol.getRed(); color_tri[1] = blendcol.getGreen(); color_tri[2] = blendcol.getBlue();

						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
					}
					else if ((*edge)->GetCornerStart() == corner1 && (*edge)->GetCornerEnd() == corner0)
					{
						Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
							interpolateEdgeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, 0.3f) :
							interpolateEdgeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, 0.3f);
						color_tri[0] = blendcol.getRed(); color_tri[1] = blendcol.getGreen(); color_tri[2] = blendcol.getBlue();

						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}						
					}
				}
			}
			
		}
	}
}

void MapGen::renderNoisyPolygons()
{
	unsigned char color_tri[3] = {255, 255, 255};

	// First Fill Ocean Polygons
	HashMap<uint32, Center*>::Iterator m (CentersMap);
    while (m.next())
    {
		Center* c = m.getValue();
		if (c && c->bOcean)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;
			for( Corner** ee = c->Corners.begin(); ee < c->Corners.end(); ++ee )
			{
				jcv_point e0, e1;
				e0.x = (*ee)->Point.x;
				e0.y = (*ee)->Point.y;
				if (ee + 1 < c->Corners.end())
				{
					e1.x = (*(ee+1))->Point.x;
					e1.y = (*(ee+1))->Point.y;
				}
				else
				{
					e1.x = c->Corners.getFirst()->Point.x;
					e1.y = c->Corners.getFirst()->Point.y;
				}

				draw_triangle( &p, &e0, &e1, MapSettingParams->image, MapX, MapY, 3, color_tri);
			}
		}
	}


	// Second Fill other Biome Polygons
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;

			if (c->bOcean)
			{
				continue;
			}
			
			for(int idx = 0; idx < c->Corners.size(); ++idx )
			{
				Corner* corner0 = c->Corners[idx];
				Corner* corner1 = (idx+1 >= c->Corners.size()) ? c->Corners[0] : c->Corners[idx+1];

				for(Edge** edge = c->Corners[idx]->Protrudes.begin(); edge < c->Corners[idx]->Protrudes.end(); ++edge)
				{
					if (!MapNoisyEdges->path0.contains((*edge)->Key) || !MapNoisyEdges->path1.contains((*edge)->Key))
						continue;

					Array<Vector2D>* path0 = MapNoisyEdges->path0[(*edge)->Key];
					Array<Vector2D>* path1 = MapNoisyEdges->path1[(*edge)->Key];

					if ((*edge)->GetCornerStart() == corner0 && (*edge)->GetCornerEnd() == corner1)
					{
						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
					}
					else if ((*edge)->GetCornerStart() == corner1 && (*edge)->GetCornerEnd() == corner0)
					{
						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}						
					}
				}
			}			
		}
	}
}


// Render the exterior of polygons: coastlines, lake shores, rivers, lava fissures.
// We draw all of these after the polygons so that polygons don't overwrite any edges.
void MapGen::renderEdges()
{
	unsigned char color_edgeline[3] = {255, 255, 255};
	unsigned char color_polygonedge[3] = {0, 0, 0};

	float fLineWidth = 1.0f;


	if (MapSettingParams->RenderMode == CVoionoiMapSetting::ePolygons)
	{
		String Str;
		// First render Polygons Edge
		HashMap<uint32, Edge*>::Iterator j (EdgesMap);
		while (j.next())
		{
			Edge* edge = j.getValue();
			if (edge)
			{
				jcv_point e0, e1;
				e0.x = edge->pVoronoiStart->Point.x;
				e0.y = edge->pVoronoiStart->Point.y;
				e1.x = edge->pVoronoiEnd->Point.x;
				e1.y = edge->pVoronoiEnd->Point.y;

				draw_line((int)e0.x, (int)e0.y, (int)e1.x, (int)e1.y, MapSettingParams->image, MapX, MapY, 3, color_polygonedge);
			}
		}
	}

	// Then render Others
	HashMap<uint32, Edge*>::Iterator i (EdgesMap);
	while (i.next())
    {
		Edge* edge = i.getValue();
		if (edge)
		{
			Center* p = edge->pDelaunayStart;
			Center* r = edge->pDelaunayEnd;

			if ( !p || !r )
				continue;
			
            if (p->bOcean != r->bOcean) 
			{				
				// One side is ocean and the other side is land -- coastline
				fLineWidth = 3.0f;
				color_edgeline[0] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorCOAST].getRed();
				color_edgeline[1] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorCOAST].getGreen();
				color_edgeline[2] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorCOAST].getBlue();
            } 
			else if ((p->bWater) != (r->bWater) && (p->Biome != Center::eBiomeIce) && r->Biome != Center::eBiomeIce)
			{
				// Lake boundary
				fLineWidth = 2.0f;
				color_edgeline[0] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorLAKESHORE].getRed();
				color_edgeline[1] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorLAKESHORE].getGreen();
				color_edgeline[2] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorLAKESHORE].getBlue();

            } 
			else if (p->bWater || r->bWater) 
			{
				// Lake interior ¨C we don't want to draw the rivers here
				continue;
            } 
			//else if (lava.lava[edge.index]) 
			//{
   //         // Lava flow
   //         graphics.lineStyle(1, colors.LAVA);
   //         } 
			else if (edge->nRiver > 0)
			{
				// River edge
				fLineWidth = sqrt((float)edge->nRiver);
				color_edgeline[0] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorRIVER].getRed();
				color_edgeline[1] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorRIVER].getGreen();
				color_edgeline[2] = CVoionoiMapSetting::LineColour[CVoionoiMapSetting::eLineColorRIVER].getBlue();
            } 
			else
			{
				// No edge
				continue;
            }

			if (MapSettingParams->RenderMode == CVoionoiMapSetting::ePolygons)
			{
				jcv_point e0, e1;
				e0.x = edge->pVoronoiStart->Point.x;
				e0.y = edge->pVoronoiStart->Point.y;
				e1.x = edge->pVoronoiEnd->Point.x;
				e1.y = edge->pVoronoiEnd->Point.y;

				draw_lineWidth((int)e0.x, (int)e0.y, (int)e1.x, (int)e1.y, fLineWidth + 0.5f, MapSettingParams->image, MapX, MapY, 3, color_edgeline);
			
				continue;
			}


			// It's at the edge of the map
			if (!MapNoisyEdges->path0.contains(edge->Key) || !MapNoisyEdges->path1.contains(edge->Key))
                continue;

			Array<Vector2D>* path0 = MapNoisyEdges->path0[edge->Key];
			Array<Vector2D>* path1 = MapNoisyEdges->path1[edge->Key];
			Vector2D p0 = path0->getFirst();
			for (int ii = 1; ii < path0->size(); ii++)
			{
				draw_lineWidth((int)p0.x, (int)p0.y, (int)path0->getUnchecked(ii).x, (int)path0->getUnchecked(ii).y, fLineWidth, MapSettingParams->image, MapX, MapY, 3, color_edgeline);
				p0 = path0->getUnchecked(ii);
			}
			for (int jj = path1->size()-1; jj >= 0; jj--)
			{
				draw_lineWidth((int)p0.x, (int)p0.y, (int)path1->getUnchecked(jj).x, (int)path1->getUnchecked(jj).y, fLineWidth, MapSettingParams->image, MapX, MapY, 3, color_edgeline);
				p0 = path1->getUnchecked(jj);
			}
		}
	}
}


Colour MapGen::getSlopeColor(Colour inColor, Center* p, Center* q, Edge* edge)
{
    Corner* r = edge->GetCornerStart();
	Corner* s = edge->GetCornerEnd();
    if (!r || !s) 
	{
        // Edge of the map
		return Center::BiomeColour[Center::eBiomeOcean];
    } 
	else if (p->bWater)
	{
        return inColor;
    }

    if (q != NULL && p != NULL && p->bWater == q->bWater)
	{
		inColor = interpolateColor(Center::BiomeColour[p->Biome], Center::BiomeColour[q->Biome], 0.4f);
	}

    Colour colorLow = interpolateColor(inColor, CVoionoiMapSetting::SlopeColour[0], 0.7f);
    Colour colorHigh = interpolateColor(inColor, CVoionoiMapSetting::SlopeColour[1], 0.3f);


	Vector3D A(p->Point.x, p->fElevation, p->Point.y);
	Vector3D B(r->Point.x, r->fElevation, r->Point.y);
	Vector3D C(s->Point.x, s->fElevation, s->Point.y);
	Vector3D normal;
	normal.Cross( C-A, B-A );
    if (normal.y < 0)
	{ 
		normal *= -1; 
	}
	normal.Normalize();



	float light = jlimit(0.0f, 1.0f, normal * MapSettingParams->lightDirection * -35.0f + 0.5f);
    if (light < 0.5f) 
		return interpolateColor(colorLow, inColor, light*2);
    else 
		return interpolateColor(inColor, colorHigh, light*2-1);
}

void MapGen::renderSlopePolygons()
{
	unsigned char color_tri[3] = {255, 255, 255};

	// First Fill Ocean Polygons
	HashMap<uint32, Center*>::Iterator m (CentersMap);
    while (m.next())
    {
		Center* c = m.getValue();
		if (c && c->bOcean)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;
			for( Corner** ee = c->Corners.begin(); ee < c->Corners.end(); ++ee )
			{
				jcv_point e0, e1;
				e0.x = (*ee)->Point.x;
				e0.y = (*ee)->Point.y;
				if (ee + 1 < c->Corners.end())
				{
					e1.x = (*(ee+1))->Point.x;
					e1.y = (*(ee+1))->Point.y;
				}
				else
				{
					e1.x = c->Corners.getFirst()->Point.x;
					e1.y = c->Corners.getFirst()->Point.y;
				}

				draw_triangle( &p, &e0, &e1, MapSettingParams->image, MapX, MapY, 3, color_tri);
			}
		}
	}

	// Second Fill other Biome Polygons
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			Colour col = Center::BiomeColour[c->Biome];
			color_tri[0] = col.getRed(); color_tri[1] = col.getGreen(); color_tri[2] = col.getBlue();

			jcv_point p;
			p.x = c->Point.x; p.y = c->Point.y;

			if (c->bOcean)
			{
				continue;
			}
			
			for(int idx = 0; idx < c->Corners.size(); ++idx )
			{
				Corner* corner0 = c->Corners[idx];
				Corner* corner1 = (idx+1 >= c->Corners.size()) ? c->Corners[0] : c->Corners[idx+1];

				for(Edge** edge = c->Corners[idx]->Protrudes.begin(); edge < c->Corners[idx]->Protrudes.end(); ++edge)
				{
					if (!MapNoisyEdges->path0.contains((*edge)->Key) || !MapNoisyEdges->path1.contains((*edge)->Key))
						continue;

					Array<Vector2D>* path0 = MapNoisyEdges->path0[(*edge)->Key];
					Array<Vector2D>* path1 = MapNoisyEdges->path1[(*edge)->Key];

					if ((*edge)->GetCornerStart() == corner0 && (*edge)->GetCornerEnd() == corner1)
					{						
						Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
							getSlopeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, *edge) :
							getSlopeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, *edge);
						color_tri[0] = blendcol.getRed(); color_tri[1] = blendcol.getGreen(); color_tri[2] = blendcol.getBlue();

						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p1, &p2, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						
					}
					else if ((*edge)->GetCornerStart() == corner1 && (*edge)->GetCornerEnd() == corner0)
					{
						Colour blendcol = ((*edge)->pDelaunayStart == c) ? 
							getSlopeColor(col, (*edge)->pDelaunayStart, (*edge)->pDelaunayEnd, *edge) :
							getSlopeColor(col, (*edge)->pDelaunayEnd, (*edge)->pDelaunayStart, *edge);
						color_tri[0] = blendcol.getRed(); color_tri[1] = blendcol.getGreen(); color_tri[2] = blendcol.getBlue();

						jcv_point p1, p2;
						p1.x = path0->getFirst().x;
						p1.y = path0->getFirst().y;
						for (int ii = 1; ii < path0->size(); ii++)
						{
							p2.x = path0->getUnchecked(ii).x;
							p2.y = path0->getUnchecked(ii).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}
						for (int jj = path1->size() - 1; jj >= 0; jj--)
						{
							p2.x = path1->getUnchecked(jj).x;
							p2.y = path1->getUnchecked(jj).y;
							draw_triangle( &p, &p2, &p1, MapSettingParams->image, MapX, MapY, 3, color_tri);
							p1.x = p2.x;
							p1.y = p2.y;
						}						
					}
				}
			}
			
		}
	}
}

// Render roads. We draw these before polygon edges, so that rivers overwrite roads.
void MapGen::renderRoads()
{
	float fRoadWidth = 1.1f;
	//ROAD1: 0x442211,
	//ROAD2: 0x553322,
	//ROAD3: 0x664433,
	Colour RoadTypeColor[3] = { Colour(0xFF442211), Colour(0xFF553322), Colour(0xFF664433) };
	unsigned char color_roads[3] = { 0, 0, 0 };

	// First draw the roads, because any other feature should draw over them.
	// Also, roads don't use the noisy lines.
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* p = i.getValue();
		if (p)
		{
			if (MapRoads->roadConnections.contains(p->Index))
			{
				if (MapRoads->roadConnections[p->Index]->size() == 2) 
				{
					// Regular road: draw a spline from one edge to the other.
					Array<Edge*> edges(p->Borders);
					for (int i = 0; i < edges.size(); i++)
					{
						Edge* edge1 = edges[i];
						if (MapRoads->road[edge1->Index] > 0)
						{
							for (int j = i+1; j < edges.size(); j++) 
							{
								Edge* edge2 = edges[j];
								if (MapRoads->road[edge2->Index] > 0)
								{
									// The spline connects the midpoints of the edges
									// and at right angles to them. In between we
									// generate two control points A and B and one
									// additional vertex C.  This usually works but not always.
									float d = 0.5f * min((edge1->Midpoint - p->Point).GetLength(), (edge2->Midpoint - p->Point).GetLength());
									Vector2D A = normalTowards(edge1, p->Point, d) + edge1->Midpoint;
									Vector2D B = normalTowards(edge2, p->Point, d) + edge2->Midpoint;
									Vector2D C = (A + B) * 0.5f;

									color_roads[0] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getRed();
									color_roads[1] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getGreen();
									color_roads[2] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getBlue();
									plotQuadBezierSegAA(edge1->Midpoint.x, edge1->Midpoint.y, A.x, A.y, C.x, C.y,
										MapSettingParams->image, MapX, MapY, 3, color_roads);

									color_roads[0] = RoadTypeColor[MapRoads->road[edge2->Index] - 1].getRed();
									color_roads[1] = RoadTypeColor[MapRoads->road[edge2->Index] - 1].getGreen();
									color_roads[2] = RoadTypeColor[MapRoads->road[edge2->Index] - 1].getBlue();
									plotQuadBezierSegAA(C.x, C.y, B.x, B.y, edge2->Midpoint.x, edge2->Midpoint.y,
										MapSettingParams->image, MapX, MapY, 3, color_roads);
								}
							}
						}
					}
				}
				else
				{
					// Intersection or dead end: draw a road spline from each edge to the center
					for (int i = 0; i < p->Borders.size(); i++)
					{
						Edge* edge1 = p->Borders[i];
						if (MapRoads->road[edge1->Index] > 0)
						{
							float d = 0.25f * (edge1->Midpoint - p->Point).GetLength();
							Vector2D A = normalTowards(edge1, p->Point, d) + edge1->Midpoint;

							color_roads[0] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getRed();
							color_roads[1] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getGreen();
							color_roads[2] = RoadTypeColor[MapRoads->road[edge1->Index] - 1].getBlue();
							plotQuadBezierSegAA(edge1->Midpoint.x, edge1->Midpoint.y, A.x, A.y, p->Point.x, p->Point.y,
								MapSettingParams->image, MapX, MapY, 3, color_roads);
						}
					}
				}
			}
		}
	}
}

// Helper function: find the normal vector across edge 'e' and make sure to point it in a direction towards 'c'.
Vector2D MapGen::normalTowards(Edge* e, Vector2D c, float len)
{
	// Rotate the v0-->v1 vector by 90 degrees:
	Vector2D n(-(e->pVoronoiEnd->Point.y - e->pVoronoiStart->Point.y), e->pVoronoiEnd->Point.x - e->pVoronoiStart->Point.x);
    // Flip it around it if doesn't point towards c
	Vector2D d  = c - e->Midpoint;
    if (n.x * d.x + n.y * d.y < 0) 
	{
        n.x = -n.x;
        n.y = -n.y;
    }
	n.Normalize();
	n = n * len;
    return n;
}

void MapGen::ExportXML(const char* xmlFileName)
{
	if (!xmlFileName)
		return;

	String valueStr;
	static char* IslandShapeStr[] = {"Radial", "Perlin", "Square", "Blob"};
	static char* PointTypeStr[] = {"Relaxed", "PoissonDisk", "Square", "Hexagon"};


	pugi::xml_document doc;
	pugi::xml_node root = doc.append_child("map");

	valueStr = String(MapSettingParams->islandSeed) + String(CharPointer_UTF8("-")) + String(MapSettingParams->detailSeed);
	root.append_attribute("shape") = valueStr.getCharPointer();
	root.append_attribute("islandType") = IslandShapeStr[MapSettingParams->IslandShapeType];
	root.append_attribute("pointType") = PointTypeStr[MapSettingParams->RandomGridType];
	root.append_attribute("size") = MapSettingParams->uSitesNum;


	pugi::xml_node gen = root.append_child("generator");
	gen.append_attribute("url") = "http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/";
	gen.append_attribute("timestamp") = Time::getCurrentTime().toString(true, true, true, true).getCharPointer();
	
	pugi::xml_node cens = root.append_child("centers");
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
		Center* c = i.getValue();
		if (c)
		{
			pugi::xml_node cc = cens.append_child("center");
			cc.append_attribute("id") = c->Index;
			cc.append_attribute("x") = c->Point.x;
			cc.append_attribute("y") = c->Point.y;
			cc.append_attribute("water") = c->bWater;
			cc.append_attribute("ocean") = c->bOcean;
			cc.append_attribute("coast") = c->bCoast;
			cc.append_attribute("border") = c->bBorder;
			cc.append_attribute("biome") = c->GetBiomeName().getCharPointer();
			cc.append_attribute("elevation") = c->fElevation;
			cc.append_attribute("moisture") = c->fMoisture;


			for (int r = 0; r < c->Neighbours.size(); r++)
			{
				cc.append_child("center").append_attribute("id") = c->Neighbours[r]->Index;
            }
			for (int edge = 0; edge < c->Borders.size(); edge++)
			{
				cc.append_child("edge").append_attribute("id") = c->Borders[edge]->Index;
            }
			for (int q = 0; q <c->Corners.size(); q++)
			{
				cc.append_child("corner").append_attribute("id") = c->Corners[q]->Index;
            }
		}
	}

	pugi::xml_node eds = root.append_child("edges");
	HashMap<uint32, Edge*>::Iterator e (EdgesMap);
    while (e.next())
    {
		Edge* edge = e.getValue();
		if (edge)
		{
			pugi::xml_node eee = eds.append_child("edge");
			eee.append_attribute("id") = edge->Index;
			eee.append_attribute("river") = edge->nRiver;
			if (edge->Midpoint.x != 0 && edge->Midpoint.y != 0) 
			{
				eee.append_attribute("x") = edge->Midpoint.x;
				eee.append_attribute("y") = edge->Midpoint.y;
			}
			if (edge->pDelaunayStart)
				eee.append_attribute("center0") = edge->pDelaunayStart->Index;
			if (edge->pDelaunayEnd)
				eee.append_attribute("center1") = edge->pDelaunayEnd->Index;
			if (edge->pVoronoiStart)
				eee.append_attribute("corner0") = edge->pVoronoiStart->Index;
			if (edge->pVoronoiEnd)
				eee.append_attribute("corner1") = edge->pVoronoiEnd->Index;
        
		}
	}
	pugi::xml_node cor = root.append_child("corners");
	HashMap<uint32, Corner*>::Iterator cc (CornersMap);
    while (cc.next())
    {
		if(cc.getValue())
        {
			Corner* q = cc.getValue();
			pugi::xml_node ccc = cor.append_child("corner");
			ccc.append_attribute("id") = q->Index;
			ccc.append_attribute("x") = q->Point.x;
			ccc.append_attribute("y") = q->Point.y;
			ccc.append_attribute("water") = q->bWater;
			ccc.append_attribute("ocean") = q->bOcean;
			ccc.append_attribute("coast") = q->bCoast;
			ccc.append_attribute("border") = q->bBorder;
			ccc.append_attribute("elevation") = q->fElevation;
			ccc.append_attribute("moisture") = q->fMoisture;
			ccc.append_attribute("river") = q->River;
			ccc.append_attribute("downslope") = q->Downslope ? q->Downslope->Index : -1;

			for (int p = 0; p < q->Touches.size(); p++)
			{
				ccc.append_child("center").append_attribute("id") = q->Touches[p]->Index;
			}
			for (int edge = 0; edge < q->Protrudes.size(); edge++)
			{
				ccc.append_child("edge").append_attribute("id") = q->Protrudes[edge]->Index;
			}
			for (int s = 0; s < q->Adjacents.size(); s++)
			{
				ccc.append_child("corner").append_attribute("id") = q->Adjacents[s]->Index;
			}
		}
	}

	pugi::xml_node road = root.append_child("roads");
	HashMap<uint32, uint32>::Iterator rr (MapRoads->road);
    while (rr.next())
    {
		pugi::xml_node ro = road.append_child("road");
		ro.append_attribute("edge") = rr.getKey();
		ro.append_attribute("contour") = rr.getValue();
	}
/*
	// add a custom declaration node
	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";
	decl.append_attribute("standalone") = "no";
*/
	doc.save_file(xmlFileName);
}

