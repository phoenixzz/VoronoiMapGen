#ifndef __SGP_OPENGLCAMERA_HEADER__
#define __SGP_OPENGLCAMERA_HEADER__

/**
Here is a useful class to create a opengl camera. 
This class does not use OpenGL calls to initialize the matrix. 
This routine is nice because it can be used either with Direct3D, or OpenGL (requires m_mOpenGLProjTrans matrix conversion). 
And these matrix will be very useful to OpenGL 3.0 or higher because with OpenGL 3.0, 
you must have your own matrices functions!
*/
class COpenGLCamera
{
public:
	float					m_fNear;					// near clipping plane
	float					m_fFar;						// far clipping plane
	float					m_fFov;						// FOV
    SGP_CAMERAMODE_TYPE		m_Mode;						// persp., ortho or 2d
	Vector4D				m_CameraPos;				// camera position

    Matrix4x4				m_mViewMatrix;				// view matrix
    Matrix4x4				m_mProjMatrix;				// projection matrix
	Matrix4x4				m_mViewProjMatrix;			// View X Projection matrix (z value in -1~1)


	static Matrix4x4		m_mOpenGLProjTrans;			// OpenGL projection matrix transform (convert z from 0~1 to -1~1)
	static Matrix4x4		m_mOpenGLProjTransInv;

public:
	COpenGLCamera(ISGPRenderDevice *renderdevice) : m_pRenderDevice(renderdevice),
		m_fNear(0), m_fFar(0), m_fFov(0), m_Mode(SGPCT_PERSPECTIVE), m_CameraPos(0,0,0)
	{
		m_mOpenGLProjTrans.Identity();
		m_mOpenGLProjTrans._33 = 2;
		m_mOpenGLProjTrans._43 = -1;

		m_mOpenGLProjTransInv.InverseOf(m_mOpenGLProjTrans);
	}

	// set mode for current camera, 0:=3D(perspective), 1:=3D(orthogonal) 2:=2D
	void		setCameraMode(SGP_CAMERAMODE_TYPE mode) { m_Mode = mode; }

	void		SetClippingPlanes(float fNear, float fFar);
	void		SetViewMatrix3D(const Vector4D& vcRight, const Vector4D& vcUp, 
								const Vector4D& vcDir, const Vector4D& vcEyePos);

	void		GetFrustrum(Plane *p);
	Vector4D&	GetPos() { return m_CameraPos; }
	void		GetViewMatrix(Matrix4x4& p) { memcpy(&p, &m_mViewMatrix, sizeof(Matrix4x4)); } 
	void		GetProjMatrix(Matrix4x4& p) { memcpy(&p, &m_mProjMatrix, sizeof(Matrix4x4)); }
	void		GetViewProjMatrix(Matrix4x4& p) { memcpy(&p, &m_mViewProjMatrix, sizeof(Matrix4x4)); }
	
		// (fAspect = viewport height / width)
    void		CalcPerspProjMatrix(float fAspect);
    void		CalcOrthoProjMatrix(float l, float r, float b, float t, float fN, float fF);
	void		CalcOrthoProjMatrix(float w, float h, float fN, float fF);
	
	// 2D MODE
	void		Prepare2DMode();
private:
	COpenGLCamera();	
	void		CalcViewProjMatrix(void) 
	{ 
		m_mViewProjMatrix = m_mViewMatrix * m_mProjMatrix;
	}


private:
	ISGPRenderDevice *m_pRenderDevice;
};

#endif		// __SGP_OPENGLCAMERA_HEADER__