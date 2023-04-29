// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainMesh.h"
#include "TripleTerrain.h"
#include "MountainShader.h"

#include "LineMesh.h"
#include "CylinderMesh.h"
#include "LSystem.h"
#include "LS_TreeLine2D.h"
#include "LS_TreeLine3D.h"
class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

	void DynamicGrid(int& x, int& z, int& iteration);
	void RenderTerrain();
private:
	LightShader* shader;
	MountainShader* mountainShader;
	//TerrainMesh* m_Terrain;
	TripleTerrain* tripleTerrain;

	Light* light;

	bool autoScroll = false;
	ID3D11ShaderResourceView* landTextures[4];

	void RenderLSystem();
	void LSystemGUI();
	void BuildLSystem();
	unique_ptr<LineMesh>	m_Line;
	std::vector<CylinderMesh*> m_Cylinder;
	//std::vector<CubeMesh*> m_leaf;
	CubeMesh* m_leaf;

	LSystem	lSystem;
	LSystemSets* baseSystem;
	LS_TreeLine3D* treeLine3D;
	LS_TreeLine2D* treeLine2D;

	bool debug = false;
	float debugFloat = 0.0f;
	int treeSeed = 0;
	bool aboveSnow = false;
};

#endif