#ifndef __SGP_SPARKRENDERER_HEADER__
#define __SGP_SPARKRENDERER_HEADER__

class Group;
class Particle;


// Constants defining the available blending modes
enum BlendingMode
{
	BLENDING_NONE,			/**< No blending is applied. The particles will appeared as opaque */
	BLENDING_ADD,			/**< The additive blending is useful to render particles that supposed to emit light (fire, magic spells...) */
	BLENDING_ALPHA,			/**< The alpha blending is useful to render transparent particles */
};

// Constants defining the available rendering hints
enum RenderingHint
{
	ALPHA_TEST = 1 << 0,	/**< The alpha test. Enabling it is useful when rendering fully opaque particles with fully transparent zones (a texture of ball for instance) */
	DEPTH_TEST = 1 << 1,	/**< The depth test. Disabling it is useful when rendering particles with additive blending without having to sort them. Note that disabling the depth test will disable the depth write as well. */
	DEPTH_WRITE = 1 << 2,	/**< The depth write. Disabling it is useful when rendering particles with additive blending without having to sort them. Particles are still culled with the Zbuffer (when behind a wall for instance) */
};

/**
  An abstract class that renders particles

* the representation of a particle system is totally independant to its computation.

* Some renderers (or renderers modes) may need some buffers be attached to the Group of particles they render.
* rendering buffers are attached to groups but used by renderers. Their creation can also be given to renderer when needed.
* By enabling the buffer creation with the static method enableBuffersCreation(bool), the renderer will create the buffers he needs,
* if not already created in the group, before rendering. If buffer creation is disabled, a group that dont have the correct buffers for the renderer, 
  the render method of the renderer will simply exit.

* Note that buffers are linked to a class of renderer, not to a given renderer object.
* Moreover buffers have an inner flag that can vary function of the states of the renderer used.
*/
class SGP_API Renderer : public Registerable, public BufferHandler
{
public :
	Renderer() : Registerable(), BufferHandler(),
		active(true), renderingHintsMask(DEPTH_TEST | DEPTH_WRITE),
		alphaThreshold(1.0f), instanceScale(1.0f), instanceAlpha(1.0f)
	{}

	virtual ~Renderer() {} 



	/**
	* An inactive Renderer will render its parent Group when a call to Group::render() is made.
	* However it can still be used manually by the user with render(Group&).
	*/
	void setActive(bool active);

	/**
	* Sets the blending mode of this renderer
	*
	* This is a generic method that allows to set most common blending modes in a generic way.
	* However renderers can implement method to give more control over the blending mode used.
	*/
	virtual void setBlending(BlendingMode blendMode) = 0;

	/**
	* Enables or disables a rendering hint
	* Note that as stated, these are only hints that may not be taken into account in all rendering APIs
	*
	*/
	virtual void enableRenderingHint(RenderingHint renderingHint, bool enable);

	/**
	* Tells the alpha threshold to use when the ALPHA_TEST is enabled
	* The operation performs by the alpha test is greater or equal to threshold
	*/
	virtual void setAlphaTestThreshold(float alphaThreshold);



	/**
	* Tells whether this Renderer is active or not
	*/
	bool isActive() const;

	/**
	* Tells whether a rendering hint is enabled or not
	*/
	virtual bool isRenderingHintEnabled(RenderingHint renderingHint) const;

	/**
	* return the alpha threhold used by the alpha test
	*/
	float getAlphaTestThreshold() const;

	/**
	* setting / return the render scale used by particle system renderer
	*/
	float getInstanceScale() const;
	void  setInstanceScale(float scale);


	/**
	* setting / return the render alpha used by particle system renderer
	*/
	float getInstanceAlpha() const;
	void  setInstanceAlpha(float alpha);


	virtual void render(const Group& group) = 0;

private:
	bool active;

	uint32 renderingHintsMask;
	float alphaThreshold;
	float instanceScale;
	float instanceAlpha;
};


inline void Renderer::setActive(bool active)
{
	this->active = active;
}

inline void Renderer::enableRenderingHint(RenderingHint renderingHint, bool enable)
{
	if( enable )
		renderingHintsMask |= renderingHint;
	else
		renderingHintsMask &= ~renderingHint;
}

inline void Renderer::setAlphaTestThreshold(float alphaThreshold)
{
	this->alphaThreshold = alphaThreshold;
}

inline bool Renderer::isActive() const
{
	return active;
}

inline bool Renderer::isRenderingHintEnabled(RenderingHint renderingHint) const
{
	return (renderingHintsMask & renderingHint) != 0;
}

inline float Renderer::getAlphaTestThreshold() const
{
	return alphaThreshold;
}

inline float Renderer::getInstanceScale() const
{
	return instanceScale;
}

inline void Renderer::setInstanceScale(float scale)
{
	this->instanceScale = scale;
}

inline float Renderer::getInstanceAlpha() const
{
	return instanceAlpha;
}

inline void Renderer::setInstanceAlpha(float alpha)
{
	this->instanceAlpha = alpha;
}

#endif		// __SGP_SPARKRENDERER_HEADER__