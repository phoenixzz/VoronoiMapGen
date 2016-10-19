#ifndef __SGP_SPARKTRANSFORMABLE_HEADER__
#define __SGP_SPARKTRANSFORMABLE_HEADER__

class Zone;


/**
* An abstract class that allows matrix transformations

* Every SPARK object that can be transformated thanks to a matrix should derive from this class.
* A Transformable stores a matrix and performs operation on object parameters function of its matrix.

* It allows for instance to transform all emitters and zones in a system with a transform matrix.
* Particles are still living in the world coordinates (unlike transforming the rendering process).

* The transforms used are continuous-in-memory homogeneous matrices with vectors being stored with their coordinates contiguous :
* [right.x right.y right.z right.w up.x up.y up.z up.w lookdir.x lookdir.y lookdir.z lookdir.w eyepos.x eyepos.y eyepos.z eyepos.w]
* (like D3D left-handed coordinate systems)
*

*/
class SGP_API Transformable
{
public :
	Transformable();
	Transformable(const Transformable& transformable);
	Transformable& operator=(const Transformable& transformable);
	virtual ~Transformable() {}

	/**
	* Sets the local transform of this Transformable
	*
	* transformMat : the transform to copy its content from
	*/
	inline void setTransform(const Matrix4x4& transformMat)
	{
		localTransform = transformMat;
		notifyForUpdate();
	}

	/* Gets the local transform of this Transformable */
	inline const Matrix4x4& getLocalTransform() const 
	{
		return localTransform;
	}

	/* Gets the world transform of this Transformable */
	inline const Matrix4x4& getWorldTransform() const
	{
		return worldTransform;
	}

	/**
	* Updates the world transform of this Transformable
	*
	* The parent transform and the local transform is used to derive the world transform.
	* If parent is NULL, the local transform is simply copied to the world transform.

	* Note that this method only updates the transform if needed
	*/
	void updateTransform(const Transformable* parent = NULL);







protected:		
	/**
	* whether this Transformable needs update or not
	*/
	inline bool isUpdateNotified() const
	{
		return lastUpdate != currentUpdate;
	}

	/**
	* Notifies the Transformable for a update need
	*
	* This method has to be called when modifying a parameter that impose the transform's recomputation.
	*/
	inline void notifyForUpdate()
	{
		++currentUpdate;
	}

	/**
	* Gets the latest parent of this Transformable
	* @return the latest parent of this Transformable or NULL
	*/
	inline const Transformable* getParentTransform() const
	{
		return parent;
	}

	/**
	* Updates all the parameters in the world coordinates
	*
	* This method can be overriden in derived classes of Transformable (By default it does nothing).
	* It is this method task to compute all parameters of the class that are dependent of the world transform.
	*/
	virtual void innerUpdateTransform() {}

	/**
	* Propagates the update of the transform to transformable children of this transformable
	*
	* This method can be overriden in derived classes of Transformable (By default it does nothing).
	* It is this method task to call the updateTransform method of transformable children of this transformable.
	*/
	virtual void propagateUpdateTransform() {}

private:
	Matrix4x4 localTransform;
	Matrix4x4 worldTransform;

	uint64 currentUpdate;
	uint64 lastUpdate;
	uint64 lastParentUpdate;

	const Transformable* parent;
};

#endif		// __SGP_SPARKTRANSFORMABLE_HEADER__