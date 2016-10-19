#ifndef __SGP_COLLISIONSET_HEADER__
#define __SGP_COLLISIONSET_HEADER__


struct CollisionTriangle 
{
	CollisionTriangle(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2, void *aData)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		auxData = aData;
	}

	Vector3D v[3];
	CollisionTriangle *next;
	void *auxData;
};

class CollNode 
{
public:
	CollNode();
	~CollNode();

	void build(SortedSet <CollisionTriangle *> &triangles, const uint32 cutWeight, const uint32 diffWeight, const uint32 coplanarWeight);
	bool intersect(const Vector3D &v0, const Vector3D &v1, Vector3D *point = NULL, bool solid = true, void **auxData = NULL) const;

	bool pushSphere(Vector3D &pos, const float radius) const;

	CollisionTriangle *aboveTriangle(const Vector3D &pos) const;

protected:
	bool isect(const Vector3D &v0, const Vector3D &v1, Vector3D *point, void **auxData) const;
	
	Vector3D normal;
	float offset;

	CollNode *front;
	CollNode *back;

	CollisionTriangle *triangle;
};


class CollisionSet 
{
public:
	CollisionSet();
	~CollisionSet();

	void release();
	void addTriangle(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2, void *auxData = NULL)
	{
		triangles.add(new CollisionTriangle(v0, v1, v2, auxData));
	}
	void build(const uint32 cutWeight = 3, const uint32 diffWeight = 1, const uint32 coplanarWeight = 2);
	bool intersect(const Vector3D &v0, const Vector3D &v1, Vector3D *point = NULL, bool solid = true, void **auxData = NULL) const;
	
	bool pushSphere(Vector3D &pos, const float radius) const;

protected:
	SortedSet <CollisionTriangle *> triangles;
	CollNode *top;
};


/**
	Check if a point is inside a triangle
*/
inline bool IsPointInsideTriangle(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, const Vector3D& p, float error=0)
{
	Vector3D v0 = p3 - p1;
	Vector3D v1 = p2 - p1;
	Vector3D v2 = p - p1;

	float dot00 = v0 * v0;
	float dot01 = v0 * v1;
	float dot02 = v0 * v2;
	float dot11 = v1 * v1;
	float dot12 = v1 * v2;

	// Compute barycentric coordinates
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= -error) && (v >= -error) && (u + v <= 1.0f+error*2);
}

/*
Converts a coordinate (in texcoord space) anywhere on a triangle to
a world space texel position. It assumes [0,0] lies at the up-left of the texture.
You can use the same function to calculate the normal (by passing the vertex normals instead of vertices).
	v1, v2, v3 are the triangle world space vertices
	t1, t2, t3 are the triangle texcoords
	p is the pixel coordinate on the texture in [0-1] range
*/
inline Vector3D TexcoordToPos(
	const Vector3D& v1, const Vector3D& v2, const Vector3D& v3,
    const Vector2D& t1, const Vector2D& t2, const Vector2D& t3,
    const Vector2D& p)
{
	float i = 1.0f / ((t2.x - t1.x) * (t3.y - t1.y) - (t2.y - t1.y) * (t3.x - t1.x));
	float s = i * ( (t3.y - t1.y) * (p.x - t1.x) - (t3.x - t1.x) * (p.y - t1.y));
	float t = i * (-(t2.y - t1.y) * (p.x - t1.x) + (t2.x - t1.x) * (p.y - t1.y));

	return Vector3D(	v1.x + s * (v2.x - v1.x) + t * (v3.x - v1.x),
						v1.y + s * (v2.y - v1.y) + t * (v3.y - v1.y),
						v1.z + s * (v2.z - v1.z) + t * (v3.z - v1.z)	);
}


#endif