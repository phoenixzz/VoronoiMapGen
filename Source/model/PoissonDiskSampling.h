#ifndef __MODEL_POISSONDISK_HEADER__
#define __MODEL_POISSONDISK_HEADER__

class PoissonDiskSampling
{
public:
	PoissonDiskSampling(void);
	~PoissonDiskSampling(void);

	PoissonDiskSampling(int _width, int _height, double _min_dist, int _point_count, int64 _seed);

	Array<Vector2D> Generate();

	struct point 
	{
		point() : x(0), y(0) {};
		point(double x_, double y_) : x(x_), y(y_) {};
		point(const point &p) : x(p.x), y(p.y) {};

		int gridIndex(double cell_size, int map_width)
		{
			int x_index = x / cell_size;
			int y_index = y / cell_size;

			return x_index + y_index * map_width;
		};

		double distance(point p)
		{
			return sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y));
		}

		double x, y;
	};


private:
	
	Array<point> m_grid;
	Array<point> m_process;
	Array<Vector2D> m_sample;

	int m_width;
	int m_height;
	double m_min_dist;
	int m_point_count;
	double m_cell_size;
	int m_grid_width;
	int m_grid_height;
	int64 m_seed;

	point generatePointAround(point _point, Random* pr);
	bool inRectangle(point _point);
	bool inNeighbourhood(point _point);
	Array<point> getCellsAround(point _point);
};

#endif		// __MODEL_POISSONDISK_HEADER__