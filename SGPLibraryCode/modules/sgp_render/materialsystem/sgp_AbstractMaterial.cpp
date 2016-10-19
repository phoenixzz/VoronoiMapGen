
Array<AbstractMaterial*> AbstractMaterial::dynamicMaterials_;

AbstractMaterial::~AbstractMaterial()
{
	SetDynamic( false );
}



void AbstractMaterial::PreRender( int pass )
{
	passes_[pass]->PreRender();
}



void AbstractMaterial::PostRender( int pass )
{
	passes_[pass]->PostRender();
}



void AbstractMaterial::SetDynamic( bool dynamic )
{
	if( dynamic != dynamic_ )
	{
		dynamic_ = dynamic;
		if( dynamic_ )
		{
			dynamicMaterials_.add(this);
		}
		else
		{
			dynamicMaterials_.removeAllInstancesOf(this);			
		}
	}
}



void AbstractMaterial::UpdateDynamicMaterials( float elapsedTime )
{
	for(int i=0; i<dynamicMaterials_.size(); i++)
	{
		dynamicMaterials_.getUnchecked(i)->Update(elapsedTime);
	}	
}
