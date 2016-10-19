#ifndef __SPG_MOVEMENTCONTROLLER_HEADER__
#define __SPG_MOVEMENTCONTROLLER_HEADER__

class CMovementController
{
public:

	CMovementController();
    virtual ~CMovementController();

    virtual void Update(float fElapsedTime) = 0;

    // accessor methods
    Vector4D& GetPos(void)			{ return m_vcPos;			}
    Vector4D& GetRight(void)		{ return m_vcRight;			}
    Vector4D& GetUp(void)			{ return m_vcUp;			}
    Vector4D& GetDir(void)			{ return m_vcDir;			}
    Vector4D& GetVelocity(void)		{ return m_vcVelocity;		}

protected:
    virtual void	RecalcAxes(void);
    virtual void	Init(void);

    Vector4D		m_vcPos;      // position
    Vector4D		m_vcRight;    // right vector
    Vector4D		m_vcUp;       // up vector
    Vector4D		m_vcDir;      // direction vector (look direction)
    Vector4D		m_vcVelocity; // velocity vector (movement direction)
    Quaternion		m_Quaternion; // quaternion for rotation

    // rotation speed around local vectors
    float			m_fRollSpeed;
    float			m_fPitchSpeed;
    float			m_fYawSpeed;

    // rotation value around local vectors
    float			m_fRotX;
    float			m_fRotY;
    float			m_fRotZ;

    // other stuff
    float			m_fThrust;
};

#endif		// __SPG_MOVEMENTCONTROLLER_HEADER__