#ifndef __SGP_SPARKREGWRAPPER_HEADER__
#define __SGP_SPARKREGWRAPPER_HEADER__

/**
* A Wrapper class that allows to use any type of object as a Registerable
*
* It simply encapsulates an object of type defined at compilation time.
* It allows to define the behavior of these Group attributes when a copy of the Group occurs.
* The user can use it to define an attribute of a custom Registerable child class that needs to act as a Registerable.

* WARNING : T must obviously not be itself a Registerable.

*/
template<class T> 
class RegWrapper : public Registerable
{
	SPARK_IMPLEMENT_REGISTERABLE(RegWrapper<T>)

public :
	RegWrapper<T>(const T& object = T());

	static RegWrapper<T>* create(const T& object = T());

	T& get();

	const T& get() const;
		
private :
	T object;
};

	
template<class T>
inline RegWrapper<T>* RegWrapper<T>::create(const T& o)
{
	RegWrapper<T>* obj = new RegWrapper<T>(o);
	registerObject(obj);
	return obj;	
}

template<class T>
inline T& RegWrapper<T>::get()
{
	return object;
}

template<class T>
inline const T& RegWrapper<T>::get() const
{
	return object;
}

#endif		// __SGP_SPARKREGWRAPPER_HEADER__