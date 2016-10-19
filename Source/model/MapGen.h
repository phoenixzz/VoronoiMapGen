#ifndef __MODEL_MAPGEN_HEADER__
#define __MODEL_MAPGEN_HEADER__



class Center;
class Edge;
class Corner;
class CVoionoiMapSetting;
class NoisyEdges;

typedef struct _jcv_diagram jcv_diagram;
typedef struct _jcv_edge jcv_edge;
typedef struct _jcv_graphedge jcv_graphedge;

class MapGen
{
public:
	HashMap<uint32, Center*> CentersMap;
	HashMap<uint32, Edge*> EdgesMap;
	HashMap<uint32, Corner*> CornersMap;
	NoisyEdges* MapNoisyEdges;


public:
	MapGen() : MapX(0), MapY(0), MapSettingParams(NULL), MapNoisyEdges(NULL) {}
	~MapGen();

	Center* CreateCenter(float ax, float ay);
	Edge* CreateEdge(Corner* begin, Corner* end, Center* Left, Center* Right);
	Corner* CreateCorner(float ax, float ay);

	void LoadMap(CVoionoiMapSetting* pMapParams);
	void ResetMap();

	void ExportXML(const char* xmlFileName);

	// Fill 3D Triangles
	void Fill3DVertex(Array<Array<SGPVertex_UPOS_VERTEXCOLOR>> &VertexList, Array<Array<uint16>> &VertexIdxList);

private:
	// Create graph
	void ImproveMapData(jcv_diagram& diagram);
	bool FixPoints(jcv_graphedge& edge);
	bool DotInMap(float x, float y)
    {
        return (x >= 0 && x < MapX) && (y >= 0 && y < MapY);
    }
	void BuildNoisyEdges();


	// ISLAND
	void CreateIsland();
	bool InLand(Vector2D p);
	bool IsLandShapeRadial(Vector2D point);
	bool IsLandShapePerlin(Vector2D point);
	bool IsLandShapeSquare(Vector2D point);
	bool IsLandShapeBlob(Vector2D point);
	void FixCentersFloodFillOceans();
	void CreatePerlinTable(int64 seed, float scale, int32 octaves, float falloff);

	// elevation calculation
	struct ElevationSorter
    {
		static int compareElements( Corner* first, Corner* second ) noexcept;
	};
	struct MoistureSorter
	{
		static int compareElements( Corner* first, Corner* second ) noexcept;
	};
	void CalculateElevation();
	void RedistributeElevation();

	// moisture
	void CalculateDownslopes();
	void CalculateWatersheds();
	void CreateRivers();
	void CalculateCornerMoisture();

	// Fill Image Color
	void DrawMap();
	void renderPolygons();
	void renderSlopePolygons();
	void renderSmoothPolygons();
	void renderNoisyPolygons();
	void renderEdges();

	// SMOOTH
	inline Colour interpolateColor(Colour Color0, Colour Color1, float f)
	{
		return Colour::fromFloatRGBA(
			Color0.getFloatRed() * (1.0f - f) + Color1.getFloatRed() * f,
			Color0.getFloatGreen() * (1.0f - f) + Color1.getFloatGreen() * f,
			Color0.getFloatBlue() * (1.0f - f) + Color1.getFloatBlue() * f,
			1.0f);
	}
	// When f==0: color0, f==1: color1
	Colour interpolateEdgeColor(Colour inColor, Center* p, Center* q, float f);	
	Colour getSlopeColor(Colour inColor, Center* p, Center* q, Edge* edge);

private:
	uint32 MapX, MapY;
	float PerlinNoiseTable[256][256];		// 0.0 - 1.0
	CVoionoiMapSetting* MapSettingParams;
};

#endif