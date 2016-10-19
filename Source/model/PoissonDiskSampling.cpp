
#include "../SGPLibraryCode/SGPHeader.h"

#include "PoissonDiskSampling.h"



PoissonDiskSampling::PoissonDiskSampling(void)
{}

PoissonDiskSampling::~PoissonDiskSampling(void)
{}

PoissonDiskSampling::PoissonDiskSampling(int _width, int _height, double _min_dist, int _point_count, int64 _seed)
{
	m_width			= _width;
	m_height		= _height;
	m_min_dist		= _min_dist;
	m_point_count	= _point_count;
	m_cell_size		= m_min_dist / 1.414214;
	m_grid_width	= ceil(m_width / m_cell_size);
	m_grid_height	= ceil(m_height / m_cell_size);
	m_seed			= _seed;
	m_grid.resize(m_grid_width * m_grid_height);
}

Array<Vector2D> PoissonDiskSampling::Generate()
{
	Random r;
	r.setSeed(m_seed);

	point first_point(r.nextInt(m_width), r.nextInt(m_height));

	m_process.add(first_point);
	m_sample.add(Vector2D(first_point.x, first_point.y));
	int first_point_x = first_point.x / m_cell_size;
	int first_point_y = first_point.y / m_cell_size;
	m_grid.set(first_point_y * m_grid_width + first_point_x, first_point);

	while( m_process.size() != 0 )
	{
		int new_point_index = r.nextInt(m_process.size());
		point new_point = m_process[new_point_index];
		m_process.removeRange(new_point_index, 1);
		
		for(int i = 0; i < m_point_count; i++)
		{
			point new_point_around = generatePointAround(new_point, &r);

			if (inRectangle(new_point_around) && !inNeighbourhood(new_point_around))
			{ 
				//	cout << "Nuevo punto: (" << new_point_around.x << ", " << new_point_around.y << ")" << endl;
				m_process.add(new_point_around);
				m_sample.add(Vector2D(new_point_around.x, new_point_around.y));
				int new_point_x = new_point_around.x/m_cell_size;
				int new_point_y = new_point_around.y/m_cell_size;
				m_grid.set(new_point_y * m_grid_width + new_point_x, new_point_around);
			}
		}
	}

	return m_sample;
}

PoissonDiskSampling::point PoissonDiskSampling::generatePointAround(point p_point, Random* pr)
{
	double r1 = pr->nextDouble();
	double r2 = pr->nextDouble();

	double radius = m_min_dist * (r1 + 1);

	double angle = 2 * 3.14159265 * r2;

	double new_x = p_point.x + radius * cos(angle);
	double new_y = p_point.y + radius * sin(angle);

	return point(new_x, new_y);
}

bool PoissonDiskSampling::inRectangle(point p_point)
{
	return (p_point.x >= 0 && p_point.y >= 0 && p_point.x < m_width && p_point.y < m_height);
}

bool PoissonDiskSampling::inNeighbourhood(point p_point)
{
	Array<PoissonDiskSampling::point> cells = getCellsAround(p_point);
	int size = cells.size();
	for (int i = 0; i < size; i++)
	{
		if(cells[i].distance(p_point) < m_min_dist)
		{
			return true;
		}
	}
	return false;
}

Array<PoissonDiskSampling::point> PoissonDiskSampling::getCellsAround(point p_point)
{
	Array<point> cells;
	int x_index = p_point.x / m_cell_size;
	int y_index = p_point.y / m_cell_size;

	int min_x = max(0, x_index - 1);
	int max_x = min(m_grid_width - 1, x_index + 1);

	int min_y = max(0, y_index - 1);
	int max_y = min(m_grid_height - 1, y_index + 1);

	for(int i = min_x; i <= max_x; i++)
	{
		for(int j = min_y; j <= max_y; j++)
		{
			if (m_grid.getUnchecked(i+j*m_grid_width).x != 0 && m_grid.getUnchecked(i+j*m_grid_width).y != 0)
			{
				cells.add(m_grid.getUnchecked(i+j*m_grid_width));
			}
		}
	}
	return cells;
}