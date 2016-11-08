#include "../SGPLibraryCode/SGPHeader.h"
#include "../VoronoiMapSetting.h"

#include "Center.h"
#include "Corner.h"
#include "Edge.h"

#include "Roads.h"


void Roads::Release()
{
	road.clear();

	HashMap<uint32, Array<Edge*>*>::Iterator j (roadConnections);
	while (j.next())
	{
		if (j.getValue() != NULL)
			delete j.getValue();
	}
	roadConnections.clear();
}

// We want to mark different elevation zones so that we can draw island-circling roads that divide the areas.
void Roads::createRoads(const HashMap<uint32, Center*> &CentersMap, CVoionoiMapSetting *pMapSetting)
{
    // Oceans and coastal polygons are the lowest contour zone
    // (1). Anything connected to contour level K, if it's below elevation threshold K, 
	// or if it's water, gets contour level K.
    // (2) Anything not assigned a contour level, and connected  to contour level K, gets contour level K+1.
	RingFIFO<Center*> queue(findMinimumPower2Number(CentersMap.size()));
	float elevationThresholds[4] = { 0.0f, 0.05f, 0.37f, 0.64f };
	HashMap<uint32, uint32> cornerContour;		// corner index -> int contour level
	HashMap<uint32, uint32> centerContour;		// center index -> int contour level
	uint32 newLevel = 0;
	
	HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			Center* p = i.getValue();
			if (p->bCoast || p->bOcean)
			{
				centerContour.set(p->Index, 1);
				queue.push(&p, 1);
			}
		}
	}

	while (queue.getSize() > 0)
	{
		Center* p = NULL;
		queue.pop(&p, 1);
		for( Center** r = p->Neighbours.begin(); r < p->Neighbours.end(); ++r )
		{
			newLevel = centerContour.contains(p->Index) ? centerContour[p->Index] : 0;
			while ((*r)->fElevation > elevationThresholds[newLevel] && !(*r)->bWater) 
			{
				// NOTE: extend the contour line past bodies of water so that roads don't terminate inside lakes.
				newLevel += 1;
            }
			if (newLevel < (centerContour.contains((*r)->Index) ? centerContour[(*r)->Index] : 999)) 
			{
				centerContour.set((*r)->Index, newLevel);
				queue.push(r, 1);
            }
		}
	}

    // A corner's contour level is the MIN of its polygons
	HashMap<uint32, Center*>::Iterator l (CentersMap);
    while (l.next())
    {
        if (l.getValue())
		{
			Center* p = l.getValue();
			for( Corner** q = p->Corners.begin(); q < p->Corners.end(); ++q )
			{
				cornerContour.set((*q)->Index, 
					min((cornerContour.contains((*q)->Index) ? cornerContour[(*q)->Index] : 999),
						(centerContour.contains(p->Index) ? centerContour[p->Index] : 999)));
			}
		}
	}

    // Roads go between polygons that have different contour levels
	HashMap<uint32, Center*>::Iterator k (CentersMap);
    while (k.next())
    {
        if (k.getValue())
		{
			Center* p = k.getValue();
			for( Edge** edge = p->Borders.begin(); edge < p->Borders.end(); ++edge )
			{

				if ((*edge)->pVoronoiStart && (*edge)->pVoronoiEnd &&
					cornerContour[(*edge)->pVoronoiStart->Index] != cornerContour[(*edge)->pVoronoiEnd->Index])
				{
					road.set((*edge)->Index,
						min(cornerContour[(*edge)->pVoronoiStart->Index], cornerContour[(*edge)->pVoronoiEnd->Index]));
					if (!roadConnections.contains(p->Index))
					{
						roadConnections.set(p->Index, new Array<Edge*>());
					}
					roadConnections[p->Index]->add(*edge);
				}
            }
        }
    }

}