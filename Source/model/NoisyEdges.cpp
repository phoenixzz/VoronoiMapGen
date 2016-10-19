

#include "../SGPLibraryCode/SGPHeader.h"
#include "../VoronoiMapSetting.h"

#include "Center.h"
#include "Corner.h"
#include "Edge.h"

#include "NoisyEdges.h"

void NoisyEdges::Release()
{
	HashMap<uint32, Array<Vector2D>*>::Iterator i (path0);
	while (i.next())
	{
		if (i.getValue() != NULL)
			delete i.getValue();
	}
	path0.clear();

	HashMap<uint32, Array<Vector2D>*>::Iterator j (path1);
	while (j.next())
	{
		if (j.getValue() != NULL)
			delete j.getValue();
	}
	path1.clear();
}

// Build noisy line paths for each of the Voronoi edges. 
// There are two noisy line paths for each edge, each covering half the distance: 
// path0 is from v0 to the midpoint and path1 is from v1 to the midpoint. 
// When drawing the polygons, one or the other must be drawn in reverse order.
void NoisyEdges::BuildNoisyEdges(const HashMap<uint32, Center*> &CentersMap, CVoionoiMapSetting *pMapSetting)
{
    HashMap<uint32, Center*>::Iterator i (CentersMap);
    while (i.next())
    {
        if (i.getValue())
		{
			Center* p = i.getValue();
			for( Edge** edge = p->Borders.begin(); edge < p->Borders.end(); ++edge )
			{
				if ((*edge)->pDelaunayStart && (*edge)->pDelaunayEnd &&
					(*edge)->pVoronoiStart && (*edge)->pVoronoiEnd &&
					!path0.contains((*edge)->Key))
				{
					float f = NOISY_LINE_TRADEOFF;
					Vector2D t = interpolate((*edge)->pVoronoiStart->Point, (*edge)->pDelaunayStart->Point, f);
					Vector2D q = interpolate((*edge)->pVoronoiStart->Point, (*edge)->pDelaunayEnd->Point, f);
					Vector2D r = interpolate((*edge)->pVoronoiEnd->Point, (*edge)->pDelaunayStart->Point, f);
					Vector2D s = interpolate((*edge)->pVoronoiEnd->Point, (*edge)->pDelaunayEnd->Point, f);

					float minLength = 10.0f;
					if ((*edge)->pDelaunayStart->Biome != (*edge)->pDelaunayEnd->Biome) minLength = 3;
					if ((*edge)->pDelaunayStart->bOcean && (*edge)->pDelaunayEnd->bOcean) minLength = 100;
					if ((*edge)->pDelaunayStart->bCoast || (*edge)->pDelaunayEnd->bCoast) minLength = 1;
					if ((*edge)->nRiver) minLength = 1;
                
					path0.set((*edge)->Key, buildNoisyLineSegments(&(pMapSetting->detailRandom), (*edge)->pVoronoiStart->Point, t, (*edge)->Midpoint, q, minLength));
					path1.set((*edge)->Key, buildNoisyLineSegments(&(pMapSetting->detailRandom), (*edge)->pVoronoiEnd->Point, s, (*edge)->Midpoint, r, minLength));
				}
            }
        }
	}
}

Vector2D NoisyEdges::interpolate(Vector2D A, Vector2D B, float f)
{
	return A * f + B * (1.0f - f);
}

void NoisyEdges::subdivide(Vector2D A, Vector2D B, Vector2D C, Vector2D D, float minLength, Random* pRand, Array<Vector2D> *pResult)
{
	if ((A - C).GetLength() < minLength || (B - D).GetLength() < minLength) 
		return;


    // Subdivide the quadrilateral
	double p = pRand->nextDouble() * 0.6 + 0.2;  // vertical (along A-D and B-C)
    double q = pRand->nextDouble() * 0.6 + 0.2;  // horizontal (along A-B and D-C)

    // Midpoints
    Vector2D E = interpolate(A, D, p);
    Vector2D F = interpolate(B, C, p);
    Vector2D G = interpolate(A, B, q);
    Vector2D I = interpolate(D, C, q);
        
    // Central point
    Vector2D H = interpolate(E, F, q);
        
    // Divide the quad into subquads, but meet at H
	double s = 1.0 - (pRand->nextDouble() * 0.8 - 0.4);
    double t = 1.0 - (pRand->nextDouble() * 0.8 - 0.4);

    subdivide(A, interpolate(G, B, s), H, interpolate(E, D, t), minLength, pRand, pResult);
    pResult->add(H);
    subdivide(H, interpolate(F, C, s), C, interpolate(I, D, t), minLength, pRand, pResult);
}

// Helper function: build a single noisy line in a quadrilateral A-B-C-D,
// and store the output points in a Vector.
Array<Vector2D>* NoisyEdges::buildNoisyLineSegments(Random* pRand, Vector2D A, Vector2D B, Vector2D C, Vector2D D, float minLength)
{
	Array<Vector2D> *pResult = new Array<Vector2D>();

	pResult->add(A);
	subdivide(A, B, C, D, minLength, pRand, pResult);
	pResult->add(C);

	return pResult;
}