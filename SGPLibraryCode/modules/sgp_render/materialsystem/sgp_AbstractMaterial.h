#ifndef __SGP_ABSTRACTMATERIAL_HEADER__
#define __SGP_ABSTRACTMATERIAL_HEADER__

class ISGPMaterialProperty;

// material merge modes
enum SGP_MATERIAL_MERGE
{
	MM_Add,
	MM_Blend,
	MM_Replace,
};

// RenderPass
// This class represents a render pass.
class RenderPass
{
public :
	virtual ~RenderPass() {}
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;
	virtual void AddProperty( ISGPMaterialProperty * property ) = 0;
	//virtual void UpdateTexture(uint32 texID, uint8 unit) = 0;
};


// AbstractMaterial
// NOTE: A material is considered to be dynamic if Update() should be called each frame.
//		 Thus, calling Update() on a non-dynamic material is redundant.
class AbstractMaterial
{
public :
	explicit AbstractMaterial() : 
		dynamic_( false ),
		transparent_( false ),
		mergeMode_( MM_Add )
	{
	}
	virtual ~AbstractMaterial();

	// public interface
	virtual void Clear() = 0;
	virtual void Update( float elapsedTime ) = 0;
	virtual void Clone( AbstractMaterial *pSrcMaterial ) = 0;
	inline int GetPassCount() const { return passes_.size(); }
	void PreRender( int pass );
	void PostRender( int pass );
	inline bool IsDynamic() const { return dynamic_; }					// returns true if the material is dynamic
	inline bool IsTransparent() const { return transparent_; }			// returns true if the material is transparent
	inline SGP_MATERIAL_MERGE GetMergeMode() const { return mergeMode_; }	// returns the default merging mode for this material
	static void UpdateDynamicMaterials( float elapsedTime );			// update the dynamic materials
	RenderPass* GetRenderPass(int PassID) { return passes_[PassID]; } 

protected :
	// protected interface
	void SetDynamic( bool dynamic );

	Array<RenderPass*>		passes_;

	bool					transparent_;
	SGP_MATERIAL_MERGE		mergeMode_;


private :
	static Array<AbstractMaterial*> dynamicMaterials_;

	bool dynamic_;
};


#endif		// __SGP_ABSTRACTMATERIAL_HEADER__