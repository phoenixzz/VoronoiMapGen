

// construct quaternion from Euler angles
void Quaternion::MakeFromEuler(float fPitch, float fYaw, float fRoll)
{
	float cX, cY, cZ, sX, sY, sZ, cYcZ, sYsZ, cYsZ, sYcZ;

	fPitch *= 0.5f;
	fYaw   *= 0.5f;
	fRoll  *= 0.5f;

	cX = cosf(fPitch);
	cY = cosf(fYaw);
	cZ = cosf(fRoll);

	sX = sinf(fPitch);
	sY = sinf(fYaw);
	sZ = sinf(fRoll);

	cYcZ = cY * cZ;
	sYsZ = sY * sZ;
	cYsZ = cY * sZ;
	sYcZ = sY * cZ;

	w = cX * cYcZ + sX * sYsZ;
	x = sX * cYcZ - cX * sYsZ;
	y = cX * sYcZ + sX * cYsZ;
	z = cX * cYsZ - sX * sYcZ;
}

// Computes a unit length quaternion
void Quaternion::Normalize(void)
{
	float m = sqrtf(x*x + y*y + z*z + w*w);
	if( m != 0 ) 
	{ 
		x /= m; y /= m; z /= m; w /= m;
	}
}

// Conjugate of a quaternion
void Quaternion::Conjugate(Quaternion q) 
{
	x=-q.x;  y=-q.y;  z=-q.z;  w=q.w;
}

// converts quaternion into euler angles
void Quaternion::GetEulers(float *fPitch, float *fYaw, float *fRoll)
{
	double   r11, r21, r31, r32, r33, r12, r13;
	double   q00, q11, q22, q33;
	double   tmp;

	q00 = w * w;
	q11 = x * x;
	q22 = y * y;
	q33 = z * z;

	r11 = q00 + q11 - q22 - q33;
	r21 = 2 * (x*y + w*z);
	r31 = 2 * (x*z - w*y);
	r32 = 2 * (y*z + w*x);
	r33 = q00 - q11 - q22 + q33;

	tmp = fabs(r31);
	if(tmp > 0.999999)
	{
		r12 = 2 * (x*y - w*z);
		r13 = 2 * (x*z + w*y);

		*fPitch = 0.0f;
		*fYaw   = (float)-((float_Pi/2) * r31/tmp);
		*fRoll  = (float)atan2(-r12, -r31*r13);
	}
	else 
	{
		*fPitch = (float)atan2(r32, r33);
		*fYaw   = (float)asin(-r31);
		*fRoll  = (float)atan2(r21, r11);
	}
}

// get a rotation matrix out of the quaternion
void Quaternion::GetMatrix(Matrix4x4 *pMat) 
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// set matrix to identity
	memset(pMat, 0, sizeof(Matrix4x4));
	pMat->_44 = 1.0f;

	x2 = x + x; 
	y2 = y + y; 
	z2 = z + z;

	xx = x * x2;   
	xy = x * y2;  
	xz = x * z2;

	yy = y * y2;   
	yz = y * z2;   
	zz = z * z2;

	wx = w * x2;   
	wy = w * y2;   
	wz = w * z2;

	pMat->_11 = 1.0f - (yy + zz);
	pMat->_12 = xy - wz;
	pMat->_13 = xz + wy;

	pMat->_21 = xy + wz;
	pMat->_22 = 1.0f - (xx + zz);
	pMat->_23 = yz - wx;

	pMat->_31 = xz - wy;
	pMat->_32 = yz + wx;
	pMat->_33 = 1.0f - (xx + yy);
}



// dives a quaternion by a scalar value
void Quaternion::operator /= (float f) 
{ 
	w /= f;  x /= f;  y /= f;  z /= f; 
}
Quaternion Quaternion::operator / (float f)
{
	return Quaternion(x/f, y/f, z/f, w/f);
}

// multiplies a quaternion by a scalar value
void Quaternion::operator *= (float f) 
{ 
	w *= f;  x *= f;  y *= f;  z *= f; 
}
Quaternion Quaternion::operator * (float f)
{
   return Quaternion(x*f, y*f, z*f, w*f);
}

// adds two quaternions
void Quaternion::operator += (const Quaternion &q) 
{ 
	w += q.w;  x += q.x;  y += q.y;  z += q.z; 
}
Quaternion Quaternion::operator + (const Quaternion &q) const
{
	return Quaternion(x+q.x, y+q.y, z+q.z, w+q.w);
}

// multiplies two quaternions with * operator
Quaternion Quaternion::operator * (const Quaternion &q) const
{
	Quaternion qResult;
   
	qResult.w = w*q.w - x*q.x - y*q.y - z*q.z;
	qResult.x = w*q.x + x*q.w + y*q.z - z*q.y;
	qResult.y = w*q.y + y*q.w + z*q.x - x*q.z;
	qResult.z = w*q.z + z*q.w + x*q.y - y*q.x;

	return qResult;
}

// multiplies two quaternions with *= operator
void Quaternion::operator *= (const Quaternion &q)
{
	float _x, _y, _z, _w;

	_w = w*q.w - x*q.x - y*q.y - z*q.z;
	_x = w*q.x + x*q.w + y*q.z - z*q.y;
	_y = w*q.y + y*q.w + z*q.x - x*q.z;
	_z = w*q.z + z*q.w + x*q.y - y*q.x;

	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

// linear quaternion interpolation
Quaternion& Quaternion::Lerp(const Quaternion &q2, float t)
{
	x = x * (1-t) + q2.x * t;
	y = y * (1-t) + q2.y * t;
	z = z * (1-t) + q2.z * t;
	w = w * (1-t) + q2.w * t;

	Normalize();
	return *this;
}

// spherical linear interpolation
Quaternion& Quaternion::Slerp(const Quaternion &q2, float t)
{
	Quaternion q3;
	float dot = w*q2.w + x*q2.x + y*q2.y + z*q2.z;

	/*	dot = cos(theta)
	if (dot < 0), q1(this) and q2 are more than 90 degrees apart,
	so we can invert one to reduce spinning	
	*/
	if(dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	} 
	else 
		q3 = q2;

	if(dot < 0.9999f)
	{
		float angle = acosf(dot);

		float inv_d = sinf(angle*(1-t)) / sinf(angle);
		float d = sinf(angle*t) / sinf(angle);

		x = x * inv_d + q3.x * d;
		y = y * inv_d + q3.y * d;
		z = z * inv_d + q3.z * d;
		w = w * inv_d + q3.w * d;

		return *this;
	} 
	
	// if the angle is small, use linear interpolation								
	return Lerp( q3, t );
}