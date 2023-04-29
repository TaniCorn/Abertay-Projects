#pragma once
#include "PlaneMesh.h"

#include <vector>

/// <summary>
/// Single Terrain Mesh. Doesn't contain any functionality regarding to procedural generation.
/// </summary>
class TerrainMesh :
	public PlaneMesh {
public:
	TerrainMesh( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128 );
	~TerrainMesh();

	void Resize( int newResolution );
	void Regenerate( ID3D11Device* device, ID3D11DeviceContext* deviceContext );

	const inline int GetResolution(){ return terrainResolution; }//This will return the resolution set in the ui, not the actual resolution

public:
	bool resizeTerrain = false; 
public:
	
	float* changingHeightMap;

private:

	void CreateBuffers( ID3D11Device* device, VertexType* vertices, unsigned long* indices );

	bool regnerateAdditiveFunctions = false;
	int terrainResolution = 128;

	const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
	const float terrainSize = 100.0f;		//What is the width and height of our terrain



};
