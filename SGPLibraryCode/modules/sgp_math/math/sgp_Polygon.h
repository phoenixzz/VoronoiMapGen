#ifndef __SGP_POLYGON_HEADER__
#define __SGP_POLYGON_HEADER__

class Polygon 
{
	friend class Plane;		   // access for easier classifying

private:
	Plane		   m_Plane;    // plane which poly lies in

	int            m_NumP;     // number of points
	int            m_NumI;     // number of indices
	AABBox         m_Aabb;     // bounding box
	unsigned int   m_Flag;     // whatever you want it to be

	void CalcBoundingBox(void);

	//---------------------------------------

public:
	Polygon(void);
	~Polygon(void);

	//---------------------------------------

	Vector3D     *m_pPoints;  // list of points
	unsigned int *m_pIndis;   // index list

	void          Set(const Vector3D *pPoints, int nNumP, const unsigned int *pIndis, int nNumI);

	void          Clip(const Plane &_Plane, Polygon *pFront, Polygon *pBack);
	void          Clip(const AABBox &aabb);
	int           Cull(const AABBox &aabb);

	void          CopyOf( const Polygon &Poly );

	void          SwapFaces(void);

	bool          Intersects(const Ray &_Ray, bool, float *t);
	bool          Intersects(const Ray &_Ray, bool, float fL, float *t);

	int           GetNumPoints(void)      { return m_NumP;    }
	int           GetNumIndis(void)       { return m_NumI;    }
	Vector3D*     GetPoints(void)         { return m_pPoints; }
	unsigned int* GetIndices(void)        { return m_pIndis;  }
	Plane         GetPlane(void)          { return m_Plane;   }
	AABBox        GetAabb(void)           { return m_Aabb;    }
	unsigned int  GetFlag(void)           { return m_Flag;    }
	void          SetFlag(unsigned int n) { m_Flag = n;       }

};

#endif