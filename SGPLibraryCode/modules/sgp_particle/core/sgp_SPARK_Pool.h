#ifndef __SGP_SPARKPOOL_HEADER__
#define __SGP_SPARKPOOL_HEADER__

/**
* A generic container to handle a pool of objects

* A Pool is a container built upon a sgp::array.
* It allows to store continuous objects by handling active and inactive ones.

* A Pool is designed to be faster than a vector to handle. However, the order of elements is not fixed.
* A Pool holds 2 continuous list of elements : the actives and the inactives :

		When created, elements can either be activate or inactive.
		When activating an element, the first inactive element is swapped with the element to activate and the number of active elements is increased by 1.
		When inactivating an element, the last active element is swapped with the element to inactivate and the number of active elements is decreased by 1.

* So when activating or disactivating an element, a single swap is used which is faster than the displacement of all following elements when inserting or removing an element from a vector.
* Elements are not removed in a Pool but only inactivated. Typically, elements will be removed when the Pool has to be destroyed or reinitialize.

* Another difference with a vector is that the reallocation is not automatic but only manual with a call to reallocate(uint32).
* Therefore adding elements to a full Pool will have no effect until its capacity is increased.
*/

template<class T>
class Pool
{
public :
	/** the default capacity of a Pool */
	static const uint32 DEFAULT_CAPACITY = 1000;
	/**
	* Constructor of Pool
	*
	* The capacity is the maximum number of elements a Pool can hold.
	* Unlike a std vector, the capacity is not automatically increased when reaching it.
	* The user has to manually make a call to reallocate(uint32) to increase the capacity.
	* If an elements is added to a full Pool, nothing will happen.
	*
	* IN -> capacity : the maximum number of elements the Pool can hold
	*/
	Pool<T>(uint32 capacity = DEFAULT_CAPACITY) : NumOfActive(0), maxTotal(0), PoolCapacity(capacity)
	{
		container.ensureStorageAllocated(capacity);
	}

	Pool<T>(const Pool<T>& pool) : NumOfActive(pool.NumOfActive), maxTotal(0)
	{
		PoolCapacity = pool.PoolCapacity;
		container.ensureStorageAllocated(PoolCapacity);
		container = pool.container;
	}

	inline Pool<T>& operator=(const Pool<T>& pool)
	{
		if( this != &pool )
		{
			PoolCapacity = pool.PoolCapacity;
			NumOfActive = pool.NumOfActive;
			maxTotal = 0;
			container.ensureStorageAllocated(PoolCapacity);
			container = pool.container;
		}
		return *this;
	}

	/**
	* Gets the element of the Pool at index
	* Note that the index is not tested and can therefore be out of bounds.
	*/
	inline T& operator[](uint32 index)
	{
		return container.getReference(index);
	}

	// constant version of operator[](uint32).
	inline const T& operator[](uint32 index) const
	{
		return container.getReference(index);
	}

	// return the number of active elements in this Pool
	// This method is the standard equivalent to getSizeOfActive()
	inline uint32 size() const { return NumOfActive; }
	// return the number of active elements in this Pool
	inline uint32 getSizeOfActive() const { return NumOfActive; }
	// Gets the number of inactive elements in this Pool
	inline uint32 getSizeOfInactive() const { return container.size() - NumOfActive; }
	// Gets the Total number of elements in this Pool
	inline uint32 getSizeOfTotal() const { return container.size(); }


	// Gets the capacity of this Pool
	// The capacity is the maximum number of elements a Pool can hold.
	inline uint32 getSizeOfReserved() const { return PoolCapacity; }
	// Gets the room left for new elements in this Pool
	inline uint32 getSizeOfEmpty() const { return PoolCapacity - container.size(); }

	// Gets the maximum number of elements this Pool had
	// This is useful to check if the capacity is well set or not.
	inline uint32 getMaxTotal() const { return maxTotal; }

	///////////////
	// Iterators //
	///////////////
	/**
	* Returns a pointer referring to the first active element in this Pool
	* This method is the standard equivalent to beginActive().
	*/
	inline T* begin() { return container.begin(); }
	inline T* beginActive() { return container.begin(); }
	/**
	* Returns a pointer referring to the past-the-end active element in this Pool
	* This method is the standard equivalent to endActive().
	*/
	inline T* end() { return container.begin() + NumOfActive; }
	inline T* endActive() { return container.begin() + NumOfActive; }

	// Returns a pointer referring to the first inactive element in this Pool
	inline T* beginInactive() { return end(); }
	// Returns a pointer referring to the past-the-end inactive element in this Pool
	inline T* endInactive() { return container.end(); }

	/////////////////////
	// Elements access //
	/////////////////////
	/**
	* Gets the first active element
	* This method is the standard equivalent to frontActive().
	*/
	inline T& front() { return container.getReference(0); }
	inline T& frontActive() { return container.getReference(0); }
	/**
	* Gets the last active element
	* This method is the standard equivalent to backActive().
	*/
	inline T& back() { return container.getReference(NumOfActive - 1); }
	inline T& backActive() { return container.getReference(NumOfActive - 1); }
	// Gets the first inactive element
	inline T& frontInactive() { return container.getReference(NumOfActive); }
	// Gets the last inactive element
	inline T& backInactive() { return container.getReference(size()-1); }

	inline const T& front() const { return container.getReference(0); }
	inline const T& back() const { return container.getReference(NumOfActive - 1); }

	inline const T& frontActive() const { return container.getReference(0); }
	inline const T& backActive() const { return container.getReference(NumOfActive - 1); }

	inline const T& frontInactive() const  { return container.getReference(NumOfActive); }
	inline const T& backInactive() const { return container.getReference(size()-1); }



	/**
	* Fills the Pool with a copy of the passed element
	*
	* The Pool is filled entirely which means its number of elements will be equal to its capacity.
	* Note that all added elements are inactive.
	*/
	inline void assign(T& value)
	{
		container.insertMultiple(container.size()-1, value, getSizeOfEmpty() );
	}

	// Adds an active / inactive element to this Pool
	// return true if the element can be added, false otherwise (if the Pool has no more room left)
	bool pushActive(T& element)
	{
		if( (uint32)container.size() == PoolCapacity )
			return false;

		container.add(element);
		swapElements(container.getReference(NumOfActive), container.getReference(container.size()-1));
		++NumOfActive;

		if( (uint32)container.size() > maxTotal )
			maxTotal = container.size();

		return true;
	}

	bool pushInactive(T& element)
	{
		if( container.size() == PoolCapacity )
			return false;

		container.add(element);

		if( container.size() > maxTotal )
			maxTotal = container.size();

		return true;
	}

	// Inactivates an active element
	// The index is tested and if it does not point to an active element, nothing will happen.
	void makeInactive(uint32 index)
	{
		if( index >= NumOfActive )
			return;

		swapElements(container.getReference(index), container.getReference(NumOfActive - 1));
		--NumOfActive;
	}

	// Inactivates all the elements
	inline void makeAllInactive()
	{
		NumOfActive = 0;
	}

	/**
	* Activates the first inactive element
	* A pointer to the activated element is returned.
	* return a pointer to the activated element or NULL if there is no element to activate
	*/
	T* makeActive()
	{
		if( getSizeOfInactive() == 0 )
			return NULL;

		++NumOfActive;
		return &container.getReference(0) + NumOfActive - 1;
	}

	/**
	* Activates the indexed inactive element
	* The index starts at the first inactive element.
	* This method is a bit slower than makeActive() but makeActive() has the same effect as makeActive(0).
	* If the element at index is out of bounds, NULL is returned
	*/
	T* makeActive(uint32 index)
	{
		if( getSizeOfInactive() < index )
			return NULL;

		swapElements(container.getReference(NumOfActive), container.getReference(NumOfActive + index));
		++NumOfActive;
		return &container.getReference(0) + NumOfActive - 1;
	}


	/**
	* Removes an element from this Pool
	*
	* The index is checked for bounds. If it is out of bounds nothing happens.
	* Note that either an active or inactive element can be removed.
	*/
	void erase(uint32 index)
	{
		if( index >= container.size() )
			return;

		if( index < NumOfActive )
		{
			swapElements(container.getReference(index), container.getReference(NumOfActive - 1));
			--NumOfActive;
			index = NumOfActive;
		}

		swapElements(container.getReference(index), container.back());
		container.removeLast();
	}

	/** Removes all elements in this Pool */
	inline void clear()
	{
		container.clear();
		NumOfActive = 0;
	}

	/**
	* reallocates the Pool
	* This will invalidates all iterators on the Pool.
	* If the new capacity is smaller than the number of elements, nothing happens.
	*/
	inline void reallocate(uint32 capacity)
	{
		if( capacity > PoolCapacity )
		{
			PoolCapacity = capacity;
			container.ensureStorageAllocated(capacity);
		}
	}

private:
	Array<T> container;
	uint32 PoolCapacity;
	uint32 NumOfActive;
	uint32 maxTotal;

	inline void swapElements(T& a, T& b)
	{
		sgp::swapVariables(a, b);
	}
};


#endif		// __SGP_SPARKPOOL_HEADER__