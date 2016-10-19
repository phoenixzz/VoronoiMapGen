#ifndef __MODEL_NOISYEDGES_HEADER__
#define __MODEL_NOISYEDGES_HEADER__

class CVoionoiMapSetting;

class NoisyEdges 
{
public:
	const float NOISY_LINE_TRADEOFF;  // low: jagged vedge; high: jagged dedge

	HashMap<uint32, Array<Vector2D>*> path0;
	HashMap<uint32, Array<Vector2D>*> path1;

public:
	NoisyEdges() : NOISY_LINE_TRADEOFF(0.5f)
	{
		path0.clear();
		path1.clear();
	}

	~NoisyEdges()
	{
		Release();
	}

	void Release();
	void BuildNoisyEdges(const HashMap<uint32, Center*> &CentersMap, CVoionoiMapSetting *pMapSetting);

private:
	void subdivide(Vector2D A, Vector2D B, Vector2D C, Vector2D D, float minLength, Random* pRand, Array<Vector2D> *pResult);
	Vector2D interpolate(Vector2D A, Vector2D B, float f);
	Array<Vector2D>* buildNoisyLineSegments(Random* pRand, Vector2D A, Vector2D B, Vector2D C, Vector2D D, float minLength);

};

#endif		// __MODEL_NOISYEDGES_HEADER__