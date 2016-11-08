#ifndef __VORONOI_IMAGEHELPER_HEADER__
#define __VORONOI_IMAGEHELPER_HEADER__

#define JC_VORONOI_IMPLEMENTATION
// If you wish to use doubles
//#define JCV_REAL_TYPE double
//#define JCV_FABS fabs
//#define JCV_ATAN2 atan2
#include "jc_voronoi.h"


static void plot(int x, int y, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{
	if( x < 0 || y < 0 || x > (width-1) || y > (height-1) )
		return;
	int index = y * width * nchannels + x * nchannels;
	for( int i = 0; i < nchannels; ++i )
	{
		image[index+i] = color[i];
	}
}

static void plotAA(int x, int y, unsigned char* image, int width, int height, int nchannels, unsigned char* color, float alpha)
{
	if( x < 0 || y < 0 || x > (width-1) || y > (height-1) )
		return;
	int index = y * width * nchannels + x * nchannels;
	for( int i = 0; i < nchannels; ++i )
	{
		image[index+i] = (unsigned char)(color[i] * (1-alpha) + image[index+i]*alpha);
	}
}

// http://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
static inline int orient2d(const jcv_point* a, const jcv_point* b, const jcv_point* c)
{
    return (b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x);
}

static inline int min2(int a, int b)
{
	return (a < b) ? a : b;
}

static inline int max2(int a, int b)
{
	return (a > b) ? a : b;
}

static inline int min3(int a, int b, int c)
{
	return min2(a, min2(b, c));
}
static inline int max3(int a, int b, int c)
{
	return max2(a, max2(b, c));
}


//static void draw_triangle(const jcv_point* v0, const jcv_point* v1, const jcv_point* v2, unsigned char* image, int width, int height, int nchannels, unsigned char* color0, unsigned char* color1, unsigned char* color2)
//{
//	unsigned char colorr[3] = {255, 255, 255};
//
//	int area = orient2d(v0, v1, v2);
//    if( area == 0 )
//        return;	
//
//    // Compute triangle bounding box
//    int minX = min3((int)(v0->x), (int)(v1->x), (int)(v2->x));
//    int minY = min3((int)(v0->y), (int)(v1->y), (int)(v2->y));
//    int maxX = max3((int)(v0->x), (int)(v1->x), (int)(v2->x));
//    int maxY = max3((int)(v0->y), (int)(v1->y), (int)(v2->y));
//
//    // Clip against screen bounds
//    minX = max2(minX, 0);
//    minY = max2(minY, 0);
//    maxX = min2(maxX, width - 1);
//    maxY = min2(maxY, height - 1);
//
//    // Rasterize
//    jcv_point p;
//    for (p.y = minY; p.y <= maxY; p.y += 1)
//	{
//        for (p.x = minX; p.x <= maxX; p.x += 1) 
//		{
//            // Determine barycentric coordinates
//        	int w0 = orient2d(v1, v2, &p);
//        	int w1 = orient2d(v2, v0, &p);
//        	int w2 = orient2d(v0, v1, &p);
//
//            // If p is on or inside all edges, render pixel.
//            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
//            {
//				w0 = (w1 > w0 && w2 > w0) ? area - w1 - w2 : w0;
//				w1 = (w2 > w1 && w0 > w1) ? area - w0 - w2 : w1;
//				w2 = (w1 > w2 && w0 > w2) ? area - w0 - w1 : w2;
//
//				colorr[0] = jlimit(0.0f, 255.0f,
//							((float)w0 / (float)area * (float)color0[0]) +
//							((float)w1 / (float)area * (float)color1[0]) +
//							((float)w2 / (float)area * (float)color2[0]));
//
//				colorr[1] = jlimit(0.0f, 255.0f, 
//							((float)w0 / (float)area * (float)color0[1]) +
//							((float)w1 / (float)area * (float)color1[1]) +
//							((float)w2 / (float)area * (float)color2[1]));
//
//				colorr[2] = jlimit(0.0f, 255.0f, 
//							((float)w0 / (float)area * (float)color0[2]) +
//							((float)w1 / (float)area * (float)color1[2]) +
//							((float)w2 / (float)area * (float)color2[2]));
//
//                plot(p.x, p.y, image, width, height, nchannels, colorr);
//            }
//        }
//    }
//}

static void draw_triangle(const jcv_point* v0, const jcv_point* v1, const jcv_point* v2, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{
	int area = orient2d(v0, v1, v2);
    if( area == 0 )
        return;	

    // Compute triangle bounding box
    int minX = min3((int)(v0->x), (int)(v1->x), (int)(v2->x));
    int minY = min3((int)(v0->y), (int)(v1->y), (int)(v2->y));
    int maxX = max3((int)(v0->x), (int)(v1->x), (int)(v2->x));
    int maxY = max3((int)(v0->y), (int)(v1->y), (int)(v2->y));

    // Clip against screen bounds
    minX = max2(minX, 0);
    minY = max2(minY, 0);
    maxX = min2(maxX, width - 1);
    maxY = min2(maxY, height - 1);

	//if (minX == 0 || minY == 0 || maxX == width - 1 || maxY == height - 1)
	//	return;

    // Rasterize
    jcv_point p;
    for (p.y = minY; p.y <= maxY; p.y += 1)
	{
        for (p.x = minX; p.x <= maxX; p.x += 1) 
		{
            // Determine barycentric coordinates
        	int w0 = orient2d(v1, v2, &p);
        	int w1 = orient2d(v2, v0, &p);
        	int w2 = orient2d(v0, v1, &p);

            // If p is on or inside all edges, render pixel.
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                plot(p.x, p.y, image, width, height, nchannels, color);
            }
        }
    }
}

static void relax_points(const jcv_diagram* diagram, jcv_point* points)
{
	const jcv_site* sites = jcv_diagram_get_sites(diagram);
	for( int i = 0; i < diagram->numsites; ++i )
    {
		const jcv_site* site = &sites[i];
		jcv_point sum = site->p;
		int count = 1;

		const jcv_graphedge* edge = site->edges;

		while( edge )
		{
			sum.x += edge->pos[0].x;
			sum.y += edge->pos[0].y;
			++count;
			edge = edge->next;
		}

		points[site->index].x = sum.x / count;
		points[site->index].y = sum.y / count;
	}
}


// http://members.chello.at/~easyfilter/bresenham.html
static void draw_line(int x0, int y0, int x1, int y1, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{
	//if (x0 == x1 && y0 == y1)
	//	return;
	int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2; // error value e_xy

	for(;;)
	{  
		// loop
		plot(x0,y0, image, width, height, nchannels, color);
		if (x0==x1 && y0==y1) 
			break;
		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0 += sx; } // e_xy+e_x > 0
		if (e2 <= dx) { err += dx; y0 += sy; } // e_xy+e_y < 0
	}
}
/* plot an anti-aliased line of width wd */
static void draw_lineWidth(int x0, int y0, int x1, int y1, float wd, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{   
	//if (x0 == x1 && y0 == y1)
	//	return;

   int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
   int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
   int err = dx-dy, e2, x2, y2;								/* error value e_xy */
   float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

   for (wd = (wd+1)/2; ; )									/* pixel loop */
   {                                    
		float col = max2(0,255*(abs(err-dx+dy)/ed-wd+1)) / 255.0f;

		plotAA(x0, y0, image, width, height, nchannels, color, col);
		e2 = err; x2 = x0;
		if (2*e2 >= -dx) 
		{	
			/* x step */
			for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
			{
				float col1 = max2(0,255*(abs(e2)/ed-wd+1)) / 255.0f;
				plotAA(x0, y2 += sy, image, width, height, nchannels, color, col1);
			}
			if (x0 == x1)
				break;
			e2 = err; err -= dy; x0 += sx;
		}
		if (2*e2 <= dy)
		{ 
			/* y step */
			for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
			{
				float col2 = max2(0,255*(abs(e2)/ed-wd+1)) / 255.0f;
				plotAA(x2 += sx, y0, image, width, height, nchannels, color, col2);
			}
			if (y0 == y1) 
				break;
			err += dx; y0 += sy;
		}
	}
}

// draws an anti-aliased line of wd pixel width.
static void draw_lineAA(int x0, int y0, int x1, int y1, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{
	//if (x0 == x1 && y0 == y1)
	//	return;

   int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
   int err = dx-dy, e2, x2;                       /* error value e_xy */
   int ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

   for ( ; ; )
   {                                         /* pixel loop */
		float col = abs(err-dx+dy)/(float)ed;
		plotAA(x0,y0, image, width, height, nchannels, color, col);
		e2 = err; x2 = x0;
		if (2*e2 >= -dx)
		{                                    /* x step */
			if (x0 == x1) 
				break;
			if (e2+dy < ed) 
			{
				float col1 = (e2+dy)/(float)ed;
				plotAA(x0,y0+sy, image, width, height, nchannels, color, col1);
			}
			err -= dy; x0 += sx; 
		} 
		if (2*e2 <= dy) 
		{                                     /* y step */
			if (y0 == y1) break;
			if (dx-e2 < ed) 
			{
				float col2 = (dx-e2)/(float)ed;
				plotAA(x2+sx,y0, image, width, height, nchannels, color, col2);
			}
			err += dx; y0 += sy; 
		}
	}
}

/* plot a limited quadratic Bezier segment */
static void plotQuadBezierSeg(int x0, int y0, int x1, int y1, int x2, int y2, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{                                  
	int sx = x2-x1, sy = y2-y1;
	long xx = x0-x1, yy = y0-y1, xy;                  /* relative values for checks */
	double dx, dy, err, cur = xx*sy-yy*sx;                             /* curvature */

	//jassert(xx*sx <= 0 && yy*sy <= 0);		    /* sign of gradient must not change */

	if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy)		      /* begin with longer part */
	{      
		x2 = x0; x0 = sx+x1; y2 = y0; y0 = sy+y1; cur = -cur;         /* swap P0 P2 */
	}
	if (cur != 0) 
	{															/* no straight line */
		xx += sx; xx *= sx = x0 < x2 ? 1 : -1;                  /* x step direction */
		yy += sy; yy *= sy = y0 < y2 ? 1 : -1;                  /* y step direction */
		xy = 2*xx*yy; xx *= xx; yy *= yy;                 /* differences 2nd degree */
		if (cur*sx*sy < 0)
		{												      /* negated curvature? */
			xx = -xx; yy = -yy; xy = -xy; cur = -cur;
		}
		dx = 4.0*sy*cur*(x1-x0)+xx-xy;                    /* differences 1st degree */
		dy = 4.0*sx*cur*(y0-y1)+yy-xy;
		xx += xx; yy += yy; err = dx+dy+xy;                       /* error 1st step */
		do {
			plot(x0,y0, image, width, height, nchannels, color);	  /* plot curve */
			if (x0 == x2 && y0 == y2) return;       /* last pixel -> curve finished */
			y1 = 2*err < dx;                       /* save value for test of y step */
			if (2*err > dy) { x0 += sx; dx -= xy; err += dy += yy; }      /* x step */
			if (    y1    ) { y0 += sy; dy -= xy; err += dx += xx; }      /* y step */
		} while (dy < 0 && dx > 0);          /* gradient negates -> algorithm fails */
	}
	draw_line(x0,y0, x2,y2, image, width, height, nchannels, color);    /* plot remaining part to end */
}

/* draw an limited anti-aliased quadratic Bezier segment */
static void plotQuadBezierSegAA(int x0, int y0, int x1, int y1, int x2, int y2, unsigned char* image, int width, int height, int nchannels, unsigned char* color)
{                    
	int sx = x2-x1, sy = y2-y1;
	long xx = x0-x1, yy = y0-y1, xy;              /* relative values for checks */
	double dx, dy, err, ed, cur = xx*sy-yy*sx;                     /* curvature */

	//assert(xx*sx <= 0 && yy*sy <= 0);     /* sign of gradient must not change */

	if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy)		  /* begin with longer part */
	{     
		x2 = x0; x0 = sx+x1; y2 = y0; y0 = sy+y1; cur = -cur;     /* swap P0 P2 */
	}
	if (cur != 0)
	{                                                       /* no straight line */
		xx += sx; xx *= sx = x0 < x2 ? 1 : -1;              /* x step direction */
		yy += sy; yy *= sy = y0 < y2 ? 1 : -1;              /* y step direction */
		xy = 2*xx*yy; xx *= xx; yy *= yy;             /* differences 2nd degree */
		if (cur*sx*sy < 0) {                              /* negated curvature? */
			xx = -xx; yy = -yy; xy = -xy; cur = -cur;
		}
		dx = 4.0*sy*(x1-x0)*cur+xx-xy;                /* differences 1st degree */
		dy = 4.0*sx*(y0-y1)*cur+yy-xy;
		xx += xx; yy += yy; err = dx+dy+xy;                   /* error 1st step */
		do {
			cur = min(dx+xy,-xy-dy);
			ed = max(dx+xy,-xy-dy);               /* approximate error distance */
			ed += 2*ed*cur*cur/(4*ed*ed+cur*cur);
			plotAA(x0,y0,										  /* plot curve */
				image, width, height, nchannels, color, fabs(err-dx-dy-xy)/ed);          
			if (x0 == x2 || y0 == y2) break;    /* last pixel -> curve finished */
			x1 = x0; cur = dx-err; y1 = 2*err+dy < 0;
			if (2*err+dx > 0) 
			{														  /* x step */
				if (err-dy < ed) plotAA(x0,y0+sy, image, width, height, nchannels, color, fabs(err-dy)/ed);
				x0 += sx; dx -= xy; err += dy += yy;
			}
			if (y1) 
			{														  /* y step */
				if (cur < ed) 
					plotAA(x1+sx,y0, image, width, height, nchannels, color, fabs(cur)/ed);
				y0 += sy; dy -= xy; err += dx += xx;
			}
		} while (dy < dx);                  /* gradient negates -> close curves */
	}
	/* plot remaining needle to end */
	draw_lineAA(x0,y0, x2,y2, image, width, height, nchannels, color);                  
}

#endif