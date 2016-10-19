#ifndef __SGP_PARTICLESYSTEM_HEADER__
#define __SGP_PARTICLESYSTEM_HEADER__

/**
* A particle system adapted to SGP Engine

* The particle system is rendered automatically in the render process of SGP Engine
* If specified it can automatically update all particles (auto-update) on animation pass.
* It is possible to specify if the system should update particles when not visible.
* By default, auto-update is enabled only when visible.

* Note also that an ISGPParticleSystem takes care of the camera position automatically when distance computation is enabled
    on one of its Group. Therefore there is no need to call System::setCameraPosition(Vector3D).
*/
class SGP_API ISGPParticleSystem : public SPARK::System
{
	SPARK_IMPLEMENT_REGISTERABLE(ISGPParticleSystem)
public:
	/**
	*  WorldMatrix : Particle system world transform matrix (particle system position, rotation, scale)
	*  worldTransformed : true to emit particles in world, false to emit them localy
	*/
    ISGPParticleSystem(ISGPRenderDevice *pdevice, const Matrix4x4& WorldMat, bool worldTransformed = true);

	ISGPParticleSystem(const ISGPParticleSystem& system);


	static ISGPParticleSystem* create(ISGPRenderDevice *pdevice, const Matrix4x4& WorldMatrix, bool worldTransformed = true);


	/**
	* Enables or disables auto update
	*	 enableState : True to enable auto-update, false to disable it
    *	 onlyWhenVisible : True to perform auto-update only if node is visible. This parameter is ignored if auto-update is set to false.
	*/
    void setAutoUpdateEnabled(bool enableState, bool onlyWhenVisible);


	/**
	* Returns true if auto-update is enabled
	* If true, the scene node will update particles automatically, else the user must call update().
	*/
    bool isAutoUpdateEnabled() const;

	/**
	* Returns true if auto-update is performed only when visible
	*/
    bool isUpdateOnlyWhenVisible() const;

	/**
	* Tells whether this system is world transformed or not

	* If a system is transformed in the world, only its emitter and zones will be transformed.
	* The emitted particles will remain independent from the system transformation.

	* If it is transformed locally, the emitted particles will be transformed as well.
	*/
	bool isWorldTransformed() const;

	/** 
	* Gets the bounding box
	* Note that the returned bounding box is invalid if aabb computation is disabled.
	*/
    const AABBox& getBoundingBox() const;

	/**
	* Tells whether the system has finished or not
	*
	* This method will return true if :		
		There is no more active particles in the system
		All the emitters in the system are sleeping
		
	* return true if the system has finished, false otherwise
	*/
	bool hasFinished() const;

	// Updates the absolute transformation of this Particle System 
	// WorldMatrix is the world transform of this system
	void updateAbsolutePosition(const Matrix4x4& WorldMatrix);

	// Create Particle Groups array according SGPMF1ParticleTag setting
	// the array number is calculated from setting
	SPARK::Group** createParticleGroups(const SGPMF1ParticleTag& PartSetting);

	/**
	* Setting particle system scale, include emitter Zone, Modifier zone and Particle Renderer scale
	*/
	void setInstanceScale( float scale );

	/**
	* Setting particle system renderer alpha
	*/
	void setInstanceAlpha( float alpha );


	virtual bool update(float deltaTimeInSeconds);
	virtual void render() const;

private:
	ISGPRenderDevice* m_pRenderDevice;

    bool AutoUpdate;
	bool AlwaysUpdate;

	const bool worldTransformed;

	bool finished;

	Matrix4x4 worldMatrix;

	// the assignment operator is private
	ISGPParticleSystem& operator=(const ISGPParticleSystem& system);
	// This sets the right camera position if distance computation is enabled for a group of the system
	void updateCameraPosition();
	bool isVisible() const;
};


inline ISGPParticleSystem* ISGPParticleSystem::create(ISGPRenderDevice *pdevice, const Matrix4x4& WorldMatrix, bool bworldTransformed)
{
	return new ISGPParticleSystem(pdevice, WorldMatrix, bworldTransformed);
}

inline bool ISGPParticleSystem::isAutoUpdateEnabled() const
{
    return AutoUpdate;
}

inline bool ISGPParticleSystem::isUpdateOnlyWhenVisible() const
{
    return !AlwaysUpdate;
}

inline bool ISGPParticleSystem::isWorldTransformed() const
{	
	return worldTransformed;
}

inline bool ISGPParticleSystem::hasFinished() const
{
	return finished;
}

inline void ISGPParticleSystem::setAutoUpdateEnabled(bool enableState, bool onlyWhenVisible)
{
    AutoUpdate = enableState;
    AlwaysUpdate = !onlyWhenVisible;
}

inline bool ISGPParticleSystem::update(float deltaTimeInSeconds)
{
    if( AutoUpdate && ( AlwaysUpdate || isVisible() ) ) // check culling (disabled atm)
	{
		updateCameraPosition();
		finished = !System::update(deltaTimeInSeconds);
	}
	return !finished;
}
	




#endif		// __SGP_PARTICLESYSTEM_HEADER__