


void CSGPGrass::UpdateTerrainGrassChunks(CSGPTerrain* pTerrain)
{
	if( !pTerrain )
		return;

	for( int i=0; i<pTerrain->m_TerrainChunks.size(); i++ )
	{
		bool bHavingGrass = false;
		const SGPGrassCluster* pChunkGrassClusterData = pTerrain->m_TerrainChunks[i]->GetGrassClusterData();
		for( uint32 j=0; j<pTerrain->m_TerrainChunks[i]->GetGrassClusterDataCount(); j++ )
		{
			if( pChunkGrassClusterData[j].nData != 0 )
			{
				bHavingGrass = true;
				break;
			}
		}

		if( bHavingGrass )
		{
			if( !m_TerrainGrassChunks.contains(pTerrain->m_TerrainChunks[i]) )
				m_TerrainGrassChunks.add( pTerrain->m_TerrainChunks[i] );
		}
		else
			m_TerrainGrassChunks.removeFirstMatchingValue( pTerrain->m_TerrainChunks[i] );
	}
}