#ifndef __SGP_QUATERNION_HEADER__
#define __SGP_QUATERNION_HEADER__

class Quaternion 
{
public:
	float x, y, z, w;

	//---------------------------------------

	Quaternion(void)	{ x=0.0f, y=0.0f, z=0.0f, w=1.0f; }
	Quaternion(float _x, float _y, float _z, float _w)
						{ x=_x; y=_y; z=_z; w=_w; }
	Quaternion(const Quaternion &q)
	{
		x=q.x; y=q.y; z=q.z; w=q.w;
	}
	Quaternion& operator = (const Quaternion &q) 
			{ x=q.x; y=q.y; z=q.z; w=q.w; return *this; }

	void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
	void  Normalize();
	void  Conjugate(Quaternion q);
	void  GetEulers(float *fPitch, float *fYaw, float *fRoll);
	void  GetMatrix(Matrix4x4 *m);
	


	void			operator /= (float f);
	Quaternion		operator /  (float f);

	void			operator *= (float f);
	Quaternion		operator *  (float f);


	Quaternion		operator *  (const Quaternion &q) const;
	void			operator *= (const Quaternion &q);

	void			operator += (const Quaternion &q);
	Quaternion		operator +  (const Quaternion &q) const;

	Quaternion		operator -	() const { return Quaternion(-x, -y, -z, -w); }

	// linear quaternion interpolation
	Quaternion&		Lerp		(const Quaternion &q2, float t);

	// spherical linear interpolation
	Quaternion&		Slerp		(const Quaternion &q2, float t);

};

#endif