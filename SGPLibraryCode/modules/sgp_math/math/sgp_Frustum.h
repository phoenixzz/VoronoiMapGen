#ifndef __SGP_FRUSTUM_HEADER__
#define __SGP_FRUSTUM_HEADER__

class Frustum
{
public:
	enum VFPLANES
	{
		//! Far plane of the frustum. That is the plane farest away from the eye.
		VF_FAR_PLANE = 0,
		//! Near plane of the frustum. That is the plane nearest to the eye.
		VF_NEAR_PLANE,
		//! Left plane of the frustum.
		VF_LEFT_PLANE,
		//! Right plane of the frustum.
		VF_RIGHT_PLANE,
		//! Bottom plane of the frustum.
		VF_BOTTOM_PLANE,
		//! Top plane of the frustum.
		VF_TOP_PLANE,

		//! Amount of planes enclosing the view frustum. Should be 6.
		VF_PLANE_COUNT
	};


	//! all planes enclosing the view frustum.
	Plane planes[VF_PLANE_COUNT];

	//---------------------------------------

	//! Default Constructor
	Frustum(void) { /* nothing to do */ }
	inline Frustum(const Frustum& other)
	{
		for(int i=0; i<VF_PLANE_COUNT; ++i)
			planes[i] = other.planes[i];
	}

	//! This constructor creates a view frustum based on a projection and/or view matrix.
	inline Frustum(const Matrix4x4& mat)
	{
		setFrom( mat );
	}

	inline Frustum& operator=(const Frustum& other)
	{
		if (this == &other)
			return *this;
		for (int i=0; i<VF_PLANE_COUNT; ++i)
			planes[i] = other.planes[i];
		return *this;
	}

	//! This constructor creates a view frustum based on a projection and/or view matrix.
	void setFrom(const Matrix4x4& viewprojmat);

	//! transforms the frustum by the matrix
	void transform(const Matrix4x4& mat);

	//! returns the point which is on the far left upper corner inside the the view frustum.
	Vector3D getFarLeftUp() const;

	//! returns the point which is on the far left bottom corner inside the the view frustum.
	Vector3D getFarLeftDown() const;

	//! returns the point which is on the far right top corner inside the the view frustum.
	Vector3D getFarRightUp() const;

	//! returns the point which is on the far right bottom corner inside the the view frustum.
	Vector3D getFarRightDown() const;

	//! returns the point which is on the near left upper corner inside the the view frustum.
	Vector3D getNearLeftUp() const;

	//! returns the point which is on the near left bottom corner inside the the view frustum.
	Vector3D getNearLeftDown() const;

	//! returns the point which is on the near right top corner inside the the view frustum.
	Vector3D getNearRightUp() const;

	//! returns the point which is on the near right bottom corner inside the the view frustum.
	Vector3D getNearRightDown() const;


private:
	Vector3D planeIntersects( const Plane& p1, const Plane& p2, const Plane& p3 ) const;

};

#endif		// __SGP_FRUSTUM_HEADER__