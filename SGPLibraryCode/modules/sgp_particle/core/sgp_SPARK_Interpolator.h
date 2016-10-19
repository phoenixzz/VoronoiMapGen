#ifndef __SGP_SPARKINTERPOLATOR_HEADER__
#define __SGP_SPARKINTERPOLATOR_HEADER__

class Particle;


// Constants defining which type of value is used for interpolation
enum InterpolationType
{
	INTERPOLATOR_LIFETIME,	/**< Constant defining the life time as the value used to interpolate */
	INTERPOLATOR_AGE,		/**< Constant defining the age as the value used to interpolate */
	INTERPOLATOR_PARAM,		/**< Constant defining a parameter as the value used to interpolate */
	INTERPOLATOR_VELOCITY,	/**< Constant defining the square norm of the velocity as the value used to interpolate */
};

/**
* An entry in the interpolator graph
*/
struct InterpolatorEntry
{
	float x;	/**< x value of this entry */
	float y0;	/**< y first value of this entry */
	float y1;	/**< y second value of this entry */

	/**Default constructor of interpolator entry. All values are set to 0 */
	InterpolatorEntry() : x(0.0f), y0(0.0f), y1(0.0f) {}

	/**
	* Constructs an interpolator entry with y0 and y1 having the same value
	*/
	InterpolatorEntry(float x, float y) : x(x), y0(y), y1(y) {}

	InterpolatorEntry(float x, float y0, float y1) : x(x), y0(y0), y1(y1) {}

	// used internally
	InterpolatorEntry(float x) : x(x) {}
};

// forward declaration to allow the set of entries in interpolator to be constructed
bool operator<(const InterpolatorEntry& entry0, const InterpolatorEntry& entry1);





/**
* An interpolator that offers flexible control over particle parameters
*
* An interpolator is created for each parameter of a model which is set as interpolated.
* The user can get the interpolator of a parameter for a given model by calling Model::getInterpolator(ModelParam).

* An interpolator can use several types of value to interpolate a given parameter :

* the lifetime of a particle : it is defined in a range between 0 and 1, 0 being the birth of the particle and 1 being its death
* the age of a particle
* the value of another parameter of a particle (which can be any of the parameters)
* the square norm of the velocity of a particle
* 
* Here is a description of how an interpolator works :

* Internally an interpolator holds a list of entries which defines a 2D graph. The entries are sorted internally along the x axis.
* Each entry have a unique x value and 2 y values (although both y can have the same value).

* The x defines the value that will be used to interpolate the parameter value. This value depends on the type set of the interpolator.
* For instance, if the type is INTERPOLATOR_AGE, the current x value will be the age of the particle.

* Knowing the current x value, the interpolator interpolates the y value in function of the entries y values.
* An interpolator holds 2 curves : the y0 one and the y1 one.
* Each particle is given a random value between 0 and 1 which defines where between the y0 and the y1 curve the interpolated y value will be.
* The final interpolated y value will be the value of the interpolated particle parameter for this frame.

* Moreover the graph can loop or not :
* If the graph does not loop, the current x value is clamped between the minimum x and the maximum x of the graph.
* If the graph loops, the current x is recomputed to fit in the range between the minimum x and the maximum x of the graph.

* Finally, it is possible to set a variation in the offset and the scale of the current x computed :
* Each particle is given an offset and a scale to compute its current x depending on the variations set. The formula to compute the final current x is the following :
* final current x = (current x + offset) * scale
* offset being randomly generated per particle in [-offsetXVariation, +offsetXVariation]
* scale being randomly generated per particle in 1.0 + [-scaleXVariation, +scaleXVariation]

* The default values of the interpolator are the following :

	type				: INTERPOLATOR_LIFETIME
	offset x variation	: 0.0
	scale x variation	: 0.0

*/
class SGP_API Interpolator
{
	friend class Particle;
	friend class Model;

public:
	/**
	* Sets the value used to interpolate
	* 
	* See the class description for more information.
	* Note that the argument param is only used when the type is INTERPOLATOR_PARAM.
	*
	* @param type : the type of value used to interpolate
	* @param param : the parameter used to interpolate when the type is INTERPOLATOR_PARAM.
	*/
	void setType(InterpolationType type, ModelParam param = PARAM_SIZE);

	/**
	* Enables or disables the looping of the graph
	*
	* The range of the graph is defined between the entry with the minimum x and the entry with the maximum y.
	* If the looping is disabled, the x are clamped to the range.
	* If the looping is enabled, the value of x is reported in the range. It is better that the xmin and xmax have
	* the same y values so that the graph tiles perfectly.
	*
	*/
	void enableLooping(bool loop);

	/**
	* Sets the scale variation in x
	*/
	void setScaleXVariation(float scaleXVariation);
	float getScaleXVariation() const;
	/**
	* Sets the offset variation in x
	*/
	void setOffsetXVariation(float offsetXVariation);
	float getOffsetXVariation() const;


	/**
	* Gets the type of value used to interpolate
	*/
	InterpolationType getType() const;

	/**
	* Gets the parameter used to interpolate
	* Note that the parameter is only used if the type is INTERPOLATOR_PARAM
	*/
	ModelParam getInterpolatorParam() const;

	/**
	* Tells whether the looping is enabled or not
	*/
	bool isLoopingEnabled() const;
		


	SortedSet<InterpolatorEntry>& getGraph();
	const SortedSet<InterpolatorEntry>& getGraph() const;



	/**
	* Adds an entry to the graph
	* return true if the entry has been added to the graph, false if not (the graph already contains an entry with the same x)
	*/
	bool addEntry(const InterpolatorEntry& entry);

	/**
	* Adds an entry to the graph
	* return true if the entry has been added to the graph, false if not (the graph already contains an entry with the same x)
	*/
	bool addEntry(float x, float y);

	/**
	* Adds an entry to the graph
	* return true if the entry has been added to the graph, false if not (the graph already contains an entry with the same x)
	*/
	bool addEntry(float x, float y0, float y1);

	/** Clears the graph (removes all the entries) */
	void clearGraph();

	/** Generates a sinusoidal curve
	* Note that the graph is previously cleared from all its entries
	*/
	void generateSinCurve(float period, float amplitudeMin, float amplitudeMax, float offsetX, float offsetY, float startX, uint32 length, uint32 nbSamples);
		
	/** Generates a polynomial curve
	* Note that the graph is previously cleared from all its entries
	*/
	void generatePolyCurve(float constant, float linear, float quadratic, float cubic, float startX, float endX, uint32 nbSamples);

private:

	SortedSet<InterpolatorEntry> graph;

	InterpolationType type;
	ModelParam param;
	bool loopingEnabled;

	float scaleXVariation;
	float offsetXVariation;

	float interpolate(const Particle& particle, ModelParam interpolatedParam, float ratioY, float offsetX, float scaleX);
	float interpolateY(const InterpolatorEntry& entry, float ratio);

	// methods to compute X
	typedef float (Interpolator::*computeXFn)(const Particle&) const;
	static computeXFn COMPUTE_X_FN[4];

	float computeXLifeTime(const Particle& particle) const;
	float computeXAge(const Particle& particle) const;
	float computeXParam(const Particle& particle) const;
	float computeXVelocity(const Particle& particle) const;

	// Only a model can create and destroy an interpolator
	Interpolator();
	~Interpolator() {};
};


inline void Interpolator::setType(InterpolationType type, ModelParam param)
{
	this->type = type;
	this->param = param;
}

inline void Interpolator::enableLooping(bool loop)
{
	loopingEnabled = loop;
}

inline void Interpolator::setScaleXVariation(float scaleXVariation)
{
	this->scaleXVariation = scaleXVariation;
}

inline void Interpolator::setOffsetXVariation(float offsetXVariation)
{
	this->offsetXVariation = offsetXVariation;
}

inline InterpolationType Interpolator::getType() const
{
	return type;
}

inline ModelParam Interpolator::getInterpolatorParam() const
{
	return param;
}

inline bool Interpolator::isLoopingEnabled() const
{
	return loopingEnabled;
}

inline float Interpolator::getScaleXVariation() const
{
	return scaleXVariation;
}

inline float Interpolator::getOffsetXVariation() const
{
	return offsetXVariation;
}

inline SortedSet<InterpolatorEntry>& Interpolator::getGraph()
{
	return graph;
}

inline const SortedSet<InterpolatorEntry>& Interpolator::getGraph() const
{
	return graph;
}

inline bool Interpolator::addEntry(const InterpolatorEntry& entry)
{
	return graph.add(entry);
}

inline bool Interpolator::addEntry(float x,float y)
{
	return addEntry(InterpolatorEntry(x,y));
}

inline bool Interpolator::addEntry(float x, float y0, float y1)
{
	return addEntry(InterpolatorEntry(x, y0, y1));
}

inline void Interpolator::clearGraph()
{
	graph.clear();
}

inline float Interpolator::interpolateY(const InterpolatorEntry& entry, float ratio)
{
	return entry.y0 + (entry.y1 - entry.y0) * ratio;
}

/////////////////////////////////////////////////////////////
// Functions to sort the entries on the interpolator graph //
/////////////////////////////////////////////////////////////
inline bool operator<(const InterpolatorEntry& entry0, const InterpolatorEntry& entry1)
{
	return entry0.x < entry1.x;
}

inline bool operator<=(const InterpolatorEntry& entry0, const InterpolatorEntry& entry1)
{
	return entry0.x <= entry1.x;
}

inline bool operator>=(const InterpolatorEntry& entry0, const InterpolatorEntry& entry1)
{
	return entry0.x >= entry1.x;
}

inline bool operator==(const InterpolatorEntry& entry0, const InterpolatorEntry& entry1)
{
	return entry0.x == entry1.x;
}






#endif		// __SGP_SPARKINTERPOLATOR_HEADER__