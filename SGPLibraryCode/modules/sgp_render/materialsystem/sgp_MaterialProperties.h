#ifndef __SGP_MATERIALPROPERTIES_HEADER__
#define __SGP_MATERIALPROPERTIES_HEADER__

class ISGPMaterialProperty
{
public :	
	virtual ~ISGPMaterialProperty() {}
	virtual ISGPMaterialProperty * Clone() const { jassertfalse; return 0; }

	// public interface
	virtual void Begin() const = 0;
	virtual void End() const {}
	virtual bool RegisterEnd() const { return false; }
	virtual void Update( float ) {}
	//virtual void UpdateTexture(uint32 , uint8 ) {}
	virtual bool IsDynamic() const { return false; }

	static ISGPRenderDevice* m_pRenderDevice;
};

#endif		// __SGP_MATERIALPROPERTIES_HEADER__


