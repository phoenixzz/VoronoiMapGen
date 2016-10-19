

ISGPParticleSystem::ISGPParticleSystem(ISGPRenderDevice *pdevice, const Matrix4x4& WorldMat, bool _worldTransformed) :
	System(),
	m_pRenderDevice(pdevice),
	worldMatrix(WorldMat),
	worldTransformed(_worldTransformed),
	AutoUpdate(true),
	AlwaysUpdate(false),
	finished(false)
{}

ISGPParticleSystem::ISGPParticleSystem(const ISGPParticleSystem& system) :
	System(system),
	m_pRenderDevice(system.m_pRenderDevice),
	worldMatrix(system.worldMatrix),
	AutoUpdate(system.AutoUpdate),
	AlwaysUpdate(system.AlwaysUpdate),
	worldTransformed(system.worldTransformed),
	finished(system.finished)
{
}


void ISGPParticleSystem::render() const
{
    // Renders particles
    SPARK::System::render();
}



void ISGPParticleSystem::updateAbsolutePosition(const Matrix4x4& WorldMatrix)
{
	if( worldTransformed )
	{
		setTransform( WorldMatrix );

		updateTransform();
	}
}

const AABBox& ISGPParticleSystem::getBoundingBox() const
{
    return SPARK::System::getBoundingBox();
}

void ISGPParticleSystem::updateCameraPosition()
{
	for( SPARK::Group** it = groups.begin(); it < groups.end(); ++it )
	{
		if( (*it)->isDistanceComputationEnabled() )
		{
			Vector4D pos;
			m_pRenderDevice->getCamreaPosition( &pos );
			if( !worldTransformed )
			{
				Vector4D Tpos(  pos.x - worldMatrix._41,
								pos.y - worldMatrix._42,
								pos.z - worldMatrix._43 );
				worldMatrix.ApplyInverseRotation( &Tpos );
				pos.Set( Tpos.x, Tpos.y, Tpos.z );
			}
			setCameraPosition( Vector3D(pos.x, pos.y, pos.z) );
			break;
		}
	}
}

bool ISGPParticleSystem::isVisible() const
{
	return true;
}

void ISGPParticleSystem::setInstanceScale( float scale )
{
	const Array<SPARK::Group*>& ParticleGroups = getGroups();

	for(int i=0; i<ParticleGroups.size(); ++i)
	{
		// Emitter Zone scale
		const Array<SPARK::Emitter*>& ParticleEmitters = ParticleGroups[i]->getEmitters();
		for(int j=0; j<ParticleEmitters.size(); ++j)
		{
			SPARK::Zone* ParticleZone = ParticleEmitters[j]->getZone();
			ParticleZone->setZoneScale( scale );
			
			// Emitter Force
			ParticleEmitters[j]->setForce(ParticleEmitters[j]->getForceMin() * scale, ParticleEmitters[j]->getForceMax() * scale);
		}
		// Modifier Zone scale
		const Array<SPARK::Modifier*>& ParticleModifier = ParticleGroups[i]->getModifiers();
		for(int j=0; j<ParticleModifier.size(); ++j)
		{
			SPARK::Zone* ParticleZone = ParticleModifier[j]->getZone();
			ParticleZone->setZoneScale( scale );
		}

		// Renderer scale
		SPARK::Renderer* ParticleRenderer = ParticleGroups[i]->getRenderer();
		ParticleRenderer->setInstanceScale( scale );
	}
}

void ISGPParticleSystem::setInstanceAlpha( float alpha )
{
	const Array<SPARK::Group*>& ParticleGroups = getGroups();

	for(int i=0; i<ParticleGroups.size(); ++i)
	{
		SPARK::Renderer* ParticleRenderer = ParticleGroups[i]->getRenderer();
		ParticleRenderer->setInstanceAlpha( alpha );
	}
}

SPARK::Group** ISGPParticleSystem::createParticleGroups(const SGPMF1ParticleTag& PartSetting)
{
	uint32 groupCount = PartSetting.m_SystemParam.m_groupCount;
	if( groupCount != 0 )
	{
		SPARK::Group** pGroup = new SPARK::Group* [groupCount];
		for(uint32 i=0; i<groupCount; ++i)
		{
			ParticleGroupParam& groupParam = PartSetting.m_SystemParam.m_pGroupParam[i];
			
			// create Model
			ParticleModelParam& modelParam = groupParam.m_ModelParam;
			SPARK::Model* pModel = SPARK::Model::create( modelParam.m_EnableFlag, modelParam.m_MutableFlag, modelParam.m_RandomFlag, modelParam.m_InterpolatedFlag );
			pModel->setLifeTime( modelParam.m_LifeTimeMin, modelParam.m_LifeTimeMax );
			pModel->setImmortal( modelParam.m_bImmortal );

			for(uint32 j=0; j<modelParam.m_ParamCount; ++j)
			{
				ParticleRegularParam& regularParam = modelParam.m_pRegularParam[j];
				switch(regularParam.m_count)
				{
				case 1:
					pModel->setParam( (SPARK::ModelParam)regularParam.m_ModelParam, regularParam.m_data[0] );
					break;
				case 2:
					pModel->setParam( (SPARK::ModelParam)regularParam.m_ModelParam, regularParam.m_data[0], regularParam.m_data[1] );
					break;
				case 4:
					pModel->setParam( (SPARK::ModelParam)regularParam.m_ModelParam, regularParam.m_data[0], regularParam.m_data[1], regularParam.m_data[2], regularParam.m_data[3] );
					break;
				}
			}
			// Interpolator Param
			for(uint32 j=0; j<modelParam.m_InterpolatorCount; ++j)
			{
				ParticleInterpolatorParam& interpolatorParam = modelParam.m_pInterpolatorParam[j];
				switch(interpolatorParam.m_InterpolatorType)
				{
				case Interpolator_Polynomial:
					{
						ParticlePolyInterpolatorData& polyData = interpolatorParam.m_PolyData;
						SPARK::Interpolator* interpolator = pModel->getInterpolator( (SPARK::ModelParam)interpolatorParam.m_ModelParam );
						if( interpolator )
							interpolator->generatePolyCurve( polyData.m_constant, polyData.m_linear, polyData.m_quadratic, polyData.m_cubic, polyData.m_startX, polyData.m_endX, polyData.m_nbSamples );
					}
					break;
				case Interpolator_Sinusoidal:
					{
						ParticleSinInterpolatorData& sinData = interpolatorParam.m_SinData;
						SPARK::Interpolator* interpolator = pModel->getInterpolator( (SPARK::ModelParam)interpolatorParam.m_ModelParam );
						if( interpolator )
							interpolator->generateSinCurve( sinData.m_period, sinData.m_amplitudeMin, sinData.m_amplitudeMax, sinData.m_offsetX, sinData.m_offsetY, sinData.m_startX, sinData.m_length, sinData.m_nbSamples );
					}
					break;
				case Interpolator_SelfDefine:
					{
						ParticleSelfDefInterpolatorData& selfDefData = interpolatorParam.m_SelfDefData;
						SPARK::Interpolator* interpolator = pModel->getInterpolator( (SPARK::ModelParam)interpolatorParam.m_ModelParam );
						if( interpolator )
						{
							for(uint32 k=0; k<selfDefData.m_count; ++k)
								interpolator->addEntry( selfDefData.m_pEntry[k].m_x, selfDefData.m_pEntry[k].m_y0, selfDefData.m_pEntry[k].m_y1 );
						}
					}
					break;
				}
			}

			// create Group
			pGroup[i] = SPARK::Group::create( pModel, groupParam.m_Capacity );
			pGroup[i]->setGravity( Vector3D(groupParam.m_Gravity[0], groupParam.m_Gravity[1], groupParam.m_Gravity[2]) );
			pGroup[i]->setFriction( groupParam.m_Friction );
			pGroup[i]->enableAABBComputing( groupParam.m_EnableAABBComputing );
			pGroup[i]->enableDistanceComputation( groupParam.m_EnableComputeDistance );
			pGroup[i]->enableSorting( groupParam.m_EnableSorting );

			// create Render
			ParticleRenderParam& renderParam = groupParam.m_RenderParam;
			SPARK::Renderer* pRender = m_pRenderDevice->createOpenGLParticleRenderer(renderParam);
			//switch(renderParam.m_type)
			//{
			//case Render_Quad:
			//	{
			//		ParticleQuadRenderData& quadData = renderParam.m_quadData;
			//		SPARKOpenGLQuadRenderer* quadRender = SPARKOpenGLQuadRenderer::create( m_pRenderDevice, quadData.m_xScale, quadData.m_yScale );
			//		quadRender->setTexture( m_pRenderDevice->GetTextureManager()->getTextureIDByName(String(quadData.m_texPath)) );
			//		quadRender->setTexturingMode( (SPARK::TexturingMode)quadData.m_texMode );
			//		quadRender->setAtlasDimensions( quadData.m_xDimension, quadData.m_yDimension );
			//		quadRender->setOrientation( (SPARK::OrientationPreset)quadData.m_nOrientation );
			//		if(quadData.m_nOrientation==SPARK::AROUND_AXIS)
			//		{
			//			quadRender->lookVector.x=quadData.m_lookVector[0];
			//			quadRender->lookVector.y=quadData.m_lookVector[1];
			//			quadRender->lookVector.z=quadData.m_lookVector[2];
			//		}
			//		else if(quadData.m_nOrientation==SPARK::TOWARDS_POINT)
			//		{
			//			quadRender->lookVector.x=quadData.m_lookVector[0];
			//			quadRender->lookVector.y=quadData.m_lookVector[1];
			//			quadRender->lookVector.z=quadData.m_lookVector[2];
			//		}
			//		else if(quadData.m_nOrientation==SPARK::FIXED_ORIENTATION)
			//		{
			//			quadRender->lookVector.x=quadData.m_lookVector[0];
			//			quadRender->lookVector.y=quadData.m_lookVector[1];
			//			quadRender->lookVector.z=quadData.m_lookVector[2];
			//			quadRender->upVector.x=quadData.m_upVector[0];
			//			quadRender->upVector.y=quadData.m_upVector[1];
			//			quadRender->upVector.z=quadData.m_upVector[2];
			//		}
			//		pRender = quadRender;
			//	}
			//	break;
			//case Render_Line:
			//	{
			//		ParticleLineRenderData& lineData = renderParam.m_lineData;
			//		SPARKOpenGLLineRenderer* lineRender = SPARKOpenGLLineRenderer::create( m_pRenderDevice, lineData.m_length, lineData.m_width );
			//		pRender = lineRender;
			//	}
			//	break;
			//case Render_Point:
			//	{
			//		ParticlePointRenderData& pointData = renderParam.m_pointData;
			//		SPARKOpenGLPointRenderer* pointRender = SPARKOpenGLPointRenderer::create( m_pRenderDevice, pointData.m_size );
			//		pointRender->setType( (SPARK::PointType)pointData.m_type );
			//		pointRender->setTexture( m_pRenderDevice->GetTextureManager()->getTextureIDByName(String(pointData.m_texPath)) );
			//		pRender = pointRender;
			//	}
			//	break;
			//}
			jassert(pRender);
			pRender->setBlending( (SPARK::BlendingMode)renderParam.m_blendMode );
			//pRender->enableRenderingHint(renderParam.m_enableRenderHint);
			//pRender->setAlphaTestThreshold(renderParam.m_alphaTestThreshold);
			pGroup[i]->setRenderer( pRender );

			// create Emitter
			uint32 emitterCount = groupParam.m_nEmitterCount;
			for(uint32 j=0; j<emitterCount; ++j)
			{
				SPARK::Emitter* pEmitter = NULL;
				ParticleEmitterParam& emitterParam = groupParam.m_pEmitterParam[j];
				switch( emitterParam.m_EmitterType )
				{
				case Emitter_Straight:
					{
						ParticleStraightEmitterData& straightData = emitterParam.m_straightEmitterData;
						SPARK::StraightEmitter* straightEmitter = SPARK::StraightEmitter::create( Vector3D(straightData.m_direction[0],	straightData.m_direction[1],straightData.m_direction[2]) );
						pEmitter = straightEmitter;
					}
					break;
				case Emitter_Normal:
					{
						ParticleNormalEmitterData& normalData = emitterParam.m_normalEmitterData;
						SPARK::NormalEmitter* normalEmitter = SPARK::NormalEmitter::create( NULL, normalData.m_bInverted );
						pEmitter = normalEmitter;
					}
					break;
				case Emitter_Random:
					{
						SPARK::RandomEmitter* randomEmitter = SPARK::RandomEmitter::create();
						pEmitter = randomEmitter;
					}
					break;
				case Emitter_Spheric:
					{
						ParticleSphericEmitterData& sphericData = emitterParam.m_sphericEmitterData;
						SPARK::SphericEmitter* sphericEmitter = SPARK::SphericEmitter::create( Vector3D(sphericData.m_direction[0],	sphericData.m_direction[1], sphericData.m_direction[2]), sphericData.m_angleA, sphericData.m_angleB );
						pEmitter = sphericEmitter;
					}
					break;
				case Emitter_Static:
					{
						SPARK::StaticEmitter* staticEmitter = SPARK::StaticEmitter::create();
						pEmitter = staticEmitter;
					}
					break;
				}

				// create Zone
				SPARK::Zone* pZone = NULL;
				ParticleZoneParam& zoneParam = emitterParam.m_zoneParam;
				switch( zoneParam.m_ZoneType )
				{
				case Zone_Sphere:
					{
						ParticleSphereZoneData& zoneData = zoneParam.m_sphereZoneData;
						SPARK::SphereZone* pSphereZone = SPARK::SphereZone::create( Vector3D( zoneData.m_position[0], zoneData.m_position[1],zoneData.m_position[2] ), zoneData.m_radius );
						pZone = pSphereZone;
					}
					break;
				case Zone_AABox:
					{
						ParticleAABoxZoneData& zoneData = zoneParam.m_aaboxZoneData;
						SPARK::AABoxZone* pAABoxZone = SPARK::AABoxZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1], zoneData.m_position[2]), Vector3D(zoneData.m_dimension[0],zoneData.m_dimension[1],zoneData.m_dimension[2]) );
						pZone = pAABoxZone;
					}
					break;
				case Zone_Line:
					{
						ParticleLineZoneData& zoneData = zoneParam.m_lineZoneData;
						SPARK::LineZone* pLineZone = SPARK::LineZone::create( Vector3D(zoneData.m_p0[0],zoneData.m_p0[1],zoneData.m_p0[2]),	Vector3D(zoneData.m_p1[0],zoneData.m_p1[1],zoneData.m_p1[2]) );
						pZone = pLineZone;
					}
					break;
				case Zone_Plane:
					{
						ParticlePlaneZoneData& zoneData = zoneParam.m_planeZoneData;
						SPARK::PlaneZone* pPlaneZone = SPARK::PlaneZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]),	Vector3D(zoneData.m_normal[0],zoneData.m_normal[1],zoneData.m_normal[2]) );
						pZone = pPlaneZone;
					}
					break;
				case Zone_Ring:
					{
						ParticleRingZoneData& zoneData = zoneParam.m_ringZoneData;
						SPARK::RingZone* pRingZone = SPARK::RingZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]), Vector3D(zoneData.m_normal[0],zoneData.m_normal[1],zoneData.m_normal[2]), zoneData.m_minRadius, zoneData.m_maxRadius );
						pZone = pRingZone;
					}
					break;
				case Zone_Point:
					{
						ParticlePointZoneData& zoneData = zoneParam.m_pointZoneData;
						SPARK::PointZone* pPointZone = SPARK::PointZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]) );
						pZone = pPointZone;
					}
					break;
				}
				pEmitter->setZone( pZone );

				pEmitter->setFlow( emitterParam.m_Flow );
				pEmitter->setTank( emitterParam.m_Tank );
				pEmitter->setForce( emitterParam.m_ForceMin, emitterParam.m_ForceMax );

				pGroup[i]->addEmitter( pEmitter );
			}

			// create Modifier
			uint32 modifierCount = groupParam.m_nModifierCount;
			for( uint32 j=0; j<modifierCount; ++j )
			{
				ParticleModifierParam& modifierParam = groupParam.m_pModifierParam[j];
				SPARK::Modifier* pModifier = NULL;

				// create Zone
				SPARK::Zone* pZone = NULL;
				if( modifierParam.m_bCustomZone )
				{

					ParticleZoneParam& zoneParam = modifierParam.m_ZoneParam;
					switch(zoneParam.m_ZoneType)
					{
					case Zone_Sphere:
						{
							ParticleSphereZoneData& zoneData = zoneParam.m_sphereZoneData;
							SPARK::SphereZone* pSphereZone = SPARK::SphereZone::create( Vector3D( zoneData.m_position[0], zoneData.m_position[1], zoneData.m_position[2] ), zoneData.m_radius );
							pZone = pSphereZone;
						}
						break;
					case Zone_AABox:
						{
							ParticleAABoxZoneData& zoneData = zoneParam.m_aaboxZoneData;
							SPARK::AABoxZone* pAABoxZone = SPARK::AABoxZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]), Vector3D(zoneData.m_dimension[0],zoneData.m_dimension[1],zoneData.m_dimension[2]) );
							pZone = pAABoxZone;
						}
						break;
					case Zone_Line:
						{
							ParticleLineZoneData& zoneData = zoneParam.m_lineZoneData;
							SPARK::LineZone* pLineZone = SPARK::LineZone::create( Vector3D(zoneData.m_p0[0],zoneData.m_p0[1],zoneData.m_p0[2]), Vector3D(zoneData.m_p1[0],zoneData.m_p1[1],zoneData.m_p1[2]) );
							pZone = pLineZone;
						}
						break;
					case Zone_Plane:
						{
							ParticlePlaneZoneData& zoneData = zoneParam.m_planeZoneData;
							SPARK::PlaneZone* pPlaneZone = SPARK::PlaneZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]), Vector3D(zoneData.m_normal[0],zoneData.m_normal[1],zoneData.m_normal[2]) );
							pZone = pPlaneZone;
						}
						break;
					case Zone_Ring:
						{
							ParticleRingZoneData& zoneData = zoneParam.m_ringZoneData;
							SPARK::RingZone* pRingZone = SPARK::RingZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]), Vector3D(zoneData.m_normal[0],zoneData.m_normal[1],zoneData.m_normal[2]), zoneData.m_minRadius, zoneData.m_maxRadius );
							pZone = pRingZone;
						}
						break;
					case Zone_Point:
						{
							ParticlePointZoneData& zoneData = zoneParam.m_pointZoneData;
							SPARK::PointZone* pPointZone = SPARK::PointZone::create( Vector3D(zoneData.m_position[0],zoneData.m_position[1],zoneData.m_position[2]) );
							pZone = pPointZone;
						}
						break;
					}
				}

				switch(modifierParam.m_ModifierType)
				{
				case Modifier_Obstacle:
					{
						SPARK::Obstacle* pObstacle = SPARK::Obstacle::create(pZone);
						pObstacle->setBouncingRatio( modifierParam.m_ObstacleData.m_bounceRatio );
						pObstacle->setFriction( modifierParam.m_ObstacleData.m_Friction );
						pModifier = pObstacle;
					}
					break;
				case Modifier_LinearForce:
					{
						SPARK::LinearForce* pLinearForce = SPARK::LinearForce::create(pZone);
						ParticleLinearForceData& forceData = modifierParam.m_LinearForceData;
						pLinearForce->setForce( Vector3D(forceData.m_Force[0],forceData.m_Force[1],forceData.m_Force[2]) );
						pLinearForce->setFactor( (SPARK::ForceFactor)forceData.m_ForceFactorType, (SPARK::ModelParam)forceData.m_FactorParam );
						pModifier = pLinearForce;
					}
					break;
				case Modifier_PointMass:
					{
						SPARK::PointMass* pPointMass = SPARK::PointMass::create(pZone);
						ParticlePointMassData& pointData = modifierParam.m_PointMassData;
						pPointMass->setPosition( Vector3D(pointData.m_Position[0],pointData.m_Position[1],pointData.m_Position[2]) );
						pPointMass->setMass( pointData.m_Mass );
						pPointMass->setMinDistance( pointData.m_MinDistance );
						pModifier = pPointMass;
					}
				}
				pGroup[i]->addModifier( pModifier );
			}
		}
		return pGroup;
		
	}


	return NULL;
}