// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework

//Lights
#include "LightShader.h"
#include "FlatShader.h"
#include "FrankLunaShader.h"
#include "BandShader.h"

//Shadows
#include "DepthShader.h"

//Vertex Manipulation
#include "ManipulationShader.h"

//Tessellation
#include "QuadTessellationMesh.h"
#include "TriangleTessShader.h"
#include "QuadTessShader.h"

//Post Process
#include "KernalFilter.h"
#include "DirectionalBlur.h"
#include "TextureShader.h"

//Model
#include "AModel.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	void SetupLightShow();
	bool frame();

protected:
	bool render();
	bool firstPass();
	bool kernalPass();
	bool directionalBlurPass();

	bool shadowPass();
	void gui();

private:

	float quadExtra = 0;
	float cameraSpeed = 5.0f;
	///////////////////////////////LIGHTING
	MultiLightShader* currentShader;
	//LightShader* lightShader;
	//FlatShader* flatShader;
	FrankLunaShader* frankShader;
	int lightingModel = 0;
	//BandShader* bandShader;
	bool terrainShadows = true;

	Light* lightRepresent[MAX_LIGHTS];
	MultiLightShader::LightData lightsInScene[MAX_LIGHTS];
	SphereMesh* lightSphere[MAX_LIGHTS];

	///////////////////////////////SHADOWS
	float shadowBias = 0.004f;
	ShadowMap* shadowMap[MAX_LIGHTS];
	ID3D11ShaderResourceView* shadowMapSRV[MAX_LIGHTS];
	XMMATRIX shadowViewMatrix[MAX_LIGHTS];
	XMMATRIX shadowOrthoMatrix[MAX_LIGHTS];
	DepthShader* depthShader;

	///////////////////////////////OBJECTS IN SCENE
	//PlaneMesh* planeMesh;
	CubeMesh* cubeMesh;
	PlaneMesh* manipulatedMesh;
	AModel* model;

	//////////////////////////////VERTEX MANIPULATION/TESSELLATION
	QuadTessShader* quadTessShader;
	////////Note: We use arrays instead of XMFLOATS for ImGui usage
	float qtsParameters[4] = {25.0f, 25.0f, 1.0f, 1.01f};
	float minMaxLOD[2] = { 0, 5 };
	/*float scale = 25.0f;
	float amplitude = 25.0f;
	float worldStepRes = 1.0f;
	float offsetRes = 1.01f;*/
	float atten[3] = { 0.25, 0.017, 0.01 };
	float steepness = 1.0f;
	bool normalView = false;


	/////////////////////////////POST-PROCESSING
	int postProcess = 0;
	TextureShader* textureShader;
	RenderTexture* sceneRT;
	RenderTexture* kernalRT;
	OrthoMesh* kernalOM;

	KernalFilter* kernalFilter;
	int filterType = 0;
	int gDist[2] = { 1,1 };
	float horizontalTexel = 1.0f;

	DirectionalBlur* directionalShader;
	float directional[2] = { 1.0f, 0.0f };
	float dirStrength = 2.27f;
	int dirSample = 17;
	float dragAmount = 0.1f;

};

#endif