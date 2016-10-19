#ifndef __SGP_PERLINNOISE_HEADER__
#define __SGP_PERLINNOISE_HEADER__

/*
    Below codes come from the book...
    <Real Time 3D Terrain Engines Using C++ and DirectX>
    by Greg Snook
    greg@mightystudios.com
*/

/*
-----------------------------------------------------------------    
    A class to generate perlin noise in 2D    
-----------------------------------------------------------------
*/
class CPerlinNoise
{
public:
	enum
	{
		PerlinNoise_TableSize = 256,
		PerlinNoise_TableMask = PerlinNoise_TableSize-1,
	};

	inline CPerlinNoise(int64 PerlinSeed) : m_PerlinSeed(PerlinSeed) { setup(); }
	~CPerlinNoise() {}

	inline float noise(int x, int y, float scale);
	inline float noise(float x, float y, float scale);

private:
	Vector2D m_vecTable[PerlinNoise_TableSize];
	uint8 m_lut[PerlinNoise_TableSize];
	int64 m_PerlinSeed;

private:
	// Private Functions...
	inline void setup();
	inline const Vector2D& getVec(int x, int y) const;
};


inline void CPerlinNoise::setup()
{
	Random r;
	r.setSeed(m_PerlinSeed);

	float step = 6.24f / PerlinNoise_TableSize;
	float val = 0.0f;

	for(int i=0; i<PerlinNoise_TableSize; ++i)
	{
		m_vecTable[i].x = (float)sin(val);
		m_vecTable[i].y = (float)cos(val);
		val += step;

		m_lut[i] = r.nextInt() & PerlinNoise_TableMask;
	}
}

inline const Vector2D& CPerlinNoise::getVec(int x, int y) const
{
	uint8 a = m_lut[x & PerlinNoise_TableMask]; 
	uint8 b = m_lut[y & PerlinNoise_TableMask]; 
	uint8 val = m_lut[(a+b) & PerlinNoise_TableMask];
	return m_vecTable[val];
}


inline float CPerlinNoise::noise(float x, float y, float scale)
{
	Vector2D pos(x*scale, y*scale);

	float X0 = (float)std::floor(pos.x);
	float X1 = X0 + 1.0f;
	float Y0 = (float)std::floor(pos.y);
	float Y1 = Y0 + 1.0f;

	const Vector2D& v0 = getVec((int)X0, (int)Y0);
	const Vector2D& v1 = getVec((int)X0, (int)Y1);
	const Vector2D& v2 = getVec((int)X1, (int)Y0);
	const Vector2D& v3 = getVec((int)X1, (int)Y1);

	Vector2D d0(pos.x-X0, pos.y-Y0);
	Vector2D d1(pos.x-X0, pos.y-Y1);
	Vector2D d2(pos.x-X1, pos.y-Y0);
	Vector2D d3(pos.x-X1, pos.y-Y1);

	float h0 = (d0.x * v0.x)+(d0.y * v0.y);
	float h1 = (d1.x * v1.x)+(d1.y * v1.y);
	float h2 = (d2.x * v2.x)+(d2.y * v2.y);
	float h3 = (d3.x * v3.x)+(d3.y * v3.y);

	float Sx,Sy;

/*
	Perlin's original equation was faster,
	but produced artifacts in some situations
	Sx = (3*pow(d0.x,2.0f))
		-(2*pow(d0.x,3.0f));

	Sy = (3*pow(d0.y,2.0f))
		-(2*pow(d0.y,3.0f));
*/

	// the revised blend equation is considered more ideal, but is slower to compute
	Sx = (6*std::pow(d0.x,5.0f))
		-(15*std::pow(d0.x,4.0f))
		+(10*std::pow(d0.x,3.0f));

	Sy = (6*std::pow(d0.y,5.0f))
		-(15*std::pow(d0.y,4.0f))
		+(10*std::pow(d0.y,3.0f));


	float avgX0 = h0 + (Sx*(h2 - h0));
	float avgX1 = h1 + (Sx*(h3 - h1));
	float result = avgX0 + (Sy*(avgX1 - avgX0)); 

	return result;
}

inline float CPerlinNoise::noise(int x, int y, float scale)
{
	return noise((float)x, (float)y, scale);
}



#endif		// __SGP_PERLINNOISE_HEADER__