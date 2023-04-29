#pragma once
#ifndef TRIPLETERRAIN_H
#define TRIPLETERRAIN_H
#include "TM_InterfaceMASTER.h"
#include "TM_InterfaceNOISE.h"
#include "TM_InterfaceBASIC.h"
#include "TM_InterfaceFAULTING.h"
#include "TM_InterfacePARTICLEDEPOSITION.h"
#include "TM_InterfacePERLIN.h"
#include "TM_InterfaceHISTORY.h"

#include <vector>
#include "TerrainMesh.h"
#include "WaterTerrain.h"
#include "RandomBase10.h"

#include "D3D.h"
#include "DXF.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
class TripleTerrain
{
public:
	

	TripleTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 128);
	~TripleTerrain();

	void Resize(int newResolution);
	void Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	const inline int GetResolution() { return terrainResolution; }//This will return the resolution set in the ui, not the actual resolution
	/////////////////////////TERRAIN STUFF
	TerrainMesh* tm[3];//Contains MASTER, PREVIEW, COMPOSITE terrains
	WaterTerrain* water[3];//contains water terrain for all 3 above
	bool tmVisual[3];//Allows the enabling and disabling of terrain
	/////////////////////////INTERFACES
	TM_InterfaceMASTER* currentInterface = &basicInterface;
	TM_InterfaceHISTORY historyInterface;

	//All implementable interfaces and functions
	TM_InterfaceNOISE noiseInterface;
	TM_InterfaceBASIC basicInterface;
	TM_InterfaceFAULTING faultingInterface;
	TM_InterfacePARTICLEDEPOSITION particleDepositionInterface;
	TM_InterfacePERLIN perlinInterface;


	

	/// <summary>
/// Switches the interface to use and dictates the function to use in the regenerate function.
/// </summary>
	void SwitchUIFunction(int func) {
		//Remember to change the functions in the regenerate function
		switch (func)
		{
		case 0:
			currentInterface = &basicInterface;
			break;
		case 1:
			currentInterface = &noiseInterface;
			break;
		case 2:
			currentInterface = &faultingInterface;
			break;
		case 3:
			currentInterface = &particleDepositionInterface;
			break;
		case 4:
			currentInterface = &perlinInterface;
			perlinInterface.warp = false;
			break;
		case 5:
			currentInterface = &perlinInterface;
			perlinInterface.warp = true;
			break;
		default:
			break;
		}
	}

	/// <summary>
	/// Handles the GUI for "TM_InterfaceHEADER", "TM_InterfaceSmoothing", basic functionality and current interface parameters.
	/// </summary>
	void HandleGUI() {

		const ImVec4 texCol = ImVec4(0, 1, 0, 1);
		//Shows all preset options
		if (ImGui::TreeNode("Presets"))
		{
			PresetGUI();

			
			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Height Map Parameters"))
		{
			ImGui::Separator();
			ImGui::TextColored(texCol, "Terrain Options");

			ImGui::SliderInt("Terrain Resolution", &terrainResolution, 2, 1024);
			if (ImGui::Button("Resize Terrain")) {
				resizeTerrain = true;
			}
			ImGui::TextColored(texCol, "Heightmap views");
			ImGui::Checkbox("Preview Heightmap", &tmVisual[0]); TM_InterfaceMASTER::Tooltip("This shows only the function that you are manipulating without any addition from any previous functions"); ImGui::SameLine();
			ImGui::Checkbox("Composite Heightmap", &tmVisual[2]); TM_InterfaceMASTER::Tooltip("This shows a composition of the preview heightmap and your master heightmap. Essentially what your new master heightmap will be after pressing the button \" Apply Function to Master\""); ImGui::SameLine();
			ImGui::Checkbox("Master Heightmap", &tmVisual[1]); TM_InterfaceMASTER::Tooltip("Shows the current heightmap with all the history of applied functions together.");

			ImGui::TextColored(texCol, "Terrain texture options");
			TM_InterfaceMASTER::ValueChange("Snow Height Threshold", snowThreshold, -20.0f, 50.0f);
			TM_InterfaceMASTER::ValueChange2("Dirt Height Boundaries", dirtThreshold, -20, 50);
			TM_InterfaceMASTER::ValueChange("Sand Height Threshold", sandThreshold, -50.0f, 20.0f);
			//TM_InterfaceMASTER::ValueChange("Sand ", sandBF, -5.0f, 5.0f);
			//TM_InterfaceMASTER::ValueChange("Grass BF", grassBF, -5.0f, 5.0f);
			TM_InterfaceMASTER::ValueChange("Dirt angle", dirtBF, -1.0f, 1.0f);
			TM_InterfaceMASTER::ValueChange("Snow BF", snowBF, 0.0f, 5.0f);
			

			//HeightMap

			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Function Parameters"))
		{
			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.7,0,0,1), "After messing around with the options, remember to apply the function to the master heightmap and continue messing around.");
			/*const char* items[] = { "COMPOSITE HEIGHTMAP","Master Heightmap", "Preview Heightmap" };
			ImGui::Combo("Heightmap View", &viewType, items, 3);
			ImGui::Checkbox("Preview Smoothing", &prevSmooth);*/

			if (ImGui::Button("Apply Function to Master")) {
				historyInterface.AddHistoryKey(currentInterface->MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds,heightBounds[0], heightBounds[1]));
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
			if (ImGui::Button("Reset Heightmap")) {
				historyInterface.ResetHistory(); currentInterface->update = true;
				TM_InterfaceMASTER::FlattenHeightMap(tm[1]->changingHeightMap, terrainResolution);
			}
			ImGui::Separator();


			//ImGui::TextDisabled("(?)")
			ImGui::TextColored(texCol, "Smoothing Parameters"); 
			TM_InterfaceMASTER::RadioChange("Pre-Smooth", &smoothingPost, 0); TM_InterfaceMASTER::Tooltip("Pre-Smoothing will smooth out the function before adding it onto the master heightmap"); ImGui::SameLine();
			TM_InterfaceMASTER::RadioChange("Post-Smooth", &smoothingPost, 1); TM_InterfaceMASTER::Tooltip("Post-Smoothing will smooth out the master height map, after adding the current function."); ImGui::SameLine();
			TM_InterfaceMASTER::RadioChange("Smooth Master", &smoothingPost, 2); TM_InterfaceMASTER::Tooltip("Smooth Master will smooth the height map without adding the current function. Note: You must still press \"Apply Function To Master\"");
			currentInterface->SmoothGUI();
			ImGui::TextColored(texCol, "Apply Parameters");
			ImGui::TextWrapped("These options apply just before adding your preview heightmap to the master heightmap");

			ImGui::Checkbox("Ridgify", &ridgify); TM_InterfaceMASTER::Tooltip("This will get the absolute value and invert any values to create ridge like terrain");
			TM_InterfaceMASTER::RadioChange("Ridgify Preview", &ridgeType, 0); ImGui::SameLine();
			TM_InterfaceMASTER::RadioChange("Ridgify Post", &ridgeType, 1); ImGui::SameLine();
			TM_InterfaceMASTER::RadioChange("Ridgify Master", &ridgeType, 2);
			ImGui::Spacing();

			ImGui::Checkbox("Apply Bounds", &applyBounds); TM_InterfaceMASTER::Tooltip("If ticked, any preview effects will only be applied if the MASTER heightmap is currently within bounds.");
			TM_InterfaceMASTER::ValueChange2("Height Apply Bounds", heightBounds, -30, 30); 
			 
			ImGui::Separator();

			ImGui::TextColored(texCol, "Functions");

			const char* items2[] = { "Basic Waves", "Noise", "Faulting", "Particle Deposition", "Perlin Noise", "Warped Function" };
			if (ImGui::Combo("Selected Function", &selectedFunction, items2, IM_ARRAYSIZE(items2))) {
				SwitchUIFunction(selectedFunction);
				currentInterface->update = true;
			}



			currentInterface->HandleGUI();
			

			ImGui::TreePop();
		}

		
	}
	
	/// <summary>
	/// Will reset the heightmaps and parameters to a safe environment.
	/// </summary>
	/// <param name="hMap"></param>
	void SafetyReset() {

		for (int i = 0; i < 3; i++)
		{
			tm[i]->Resize(128);
			TM_InterfaceMASTER::FlattenHeightMap(tm[i]->changingHeightMap, 128);
		}
		terrainResolution = 128;
		SwitchUIFunction(1);//Goes to noise map
		noiseInterface.noiseParams.maxNoiseHeight = 1;
		currentInterface->update = true;//To call regenerate
	}
	void UseHistory();


	/// <summary>
/// For LSystem trees use, grabs a random point above the sand and optionally above the snow
/// </summary>
	XMFLOAT3 GetRandomPointOnTerrain(int planeResolution, bool aboveSnow) {
		int iter = 0;
		float height = -9999;
		int randx, randy;
		while (height < sandThreshold && iter < 200)
		{
			iter++;
			randx = rand() % 100;
			randy = rand() % 100;
			int pointx = (int)((float(randx) / 100.0f) * terrainResolution);
			int pointy = (int)((float(randy) / 100.0f) * terrainResolution);
			height = tm[1]->changingHeightMap[(pointy * terrainResolution) + pointx];
			if (!aboveSnow && height > snowThreshold)
			{
				height = -9999;
			}
		}


		return XMFLOAT3(randx, height, randy);
	}
private:





	////////Perlin Noise
	void BuildPerlinHeightMap(float* hMap) {};

	/// <summary>
	/// Applies a function to the preview heightmap
	/// </summary>
	void ApplyPreview(int funcNum);
	/// <summary>
	/// Adds together the master heightmap and preview
	/// </summary>
	void ApplyComposite();
	/// <summary>
	/// Adds the preview heightmap to self
	/// </summary>
	void ApplyMaster();

	/// <summary>
	/// When ridgeType or smoothing is 2, this will copy the master heightmoa to the preview and ridge/smooth to display the effect
	/// </summary>
	void PreviewMasterFunctions();




private:
		//int funcToUse = 0;//Describes the function
		int selectedFunction = 1;//Which function we are currently using

		bool regnerateAdditiveFunctions = false;//Clicking add to master will add the preview to the master
		int terrainResolution = 128;

		int smoothingPost = 0;//0,1,2 describe different smoothing operations, preview, post, and master
		bool ridgify = false;//Applies ridge function
		int ridgeType = 0;//0,1,2 describes different ridge operations, preview, post, and master
		bool applyBounds = false;//Applies any effect in a boundary
		float heightBounds[2] = { -999, 999 };//bounds for applyBounds

		const float m_UVscale = 10.0f;			//Tile the UV map 10 times across the plane
		const float terrainSize = 100.0f;		//What is the width and height of our terrain
		bool prevSmooth = false;
public:
	bool resizeTerrain = false;
	float snowThreshold = 5.0f;
	float dirtThreshold[2] = { -20.f, 20.0f };
	float sandThreshold = -1.0f;
	float sandBF = 1;
	float grassBF = 1;
	float dirtBF = 0.737;
	float snowBF = 1;

private:

#pragma region PRESETS
		void CleanPreset() {
			//Call all default params

			//terrainResolution = 200;
			smoothingPost = 0;
			ridgify = false;
			applyBounds = false;
			//Resize(terrainResolution);

			TM_InterfaceMASTER::FlattenHeightMap(tm[1]->changingHeightMap, terrainResolution);
		}
		void Preset_Marshlands(float landIncrease = 0.0f, float swampDecrease = 0.0f) {

			CleanPreset();
			particleDepositionInterface.pdParams.emitters.clear();
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			//Adds a sparse positive
			particleDepositionInterface.pdParams.emitterDensity = 0.02f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = 0.1f + landIncrease;
			particleDepositionInterface.pdParams.heightParameters[1] = 0.5f + landIncrease;
			particleDepositionInterface.pdParams.iterations = 8;
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);


			//Adds a sparse negative
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			particleDepositionInterface.pdParams.emitterDensity = 0.02f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = -0.1f - swampDecrease;
			particleDepositionInterface.pdParams.heightParameters[1] = -0.5f - swampDecrease;
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);

			smoothingPost = 0;
			currentInterface->smoothingAmount = 6;
			regnerateAdditiveFunctions = true;
			currentInterface->update = true;
			ApplyPreview(3);
			ApplyMaster();
			historyInterface.AddHistoryKey(particleDepositionInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));
			//Reduce water and smooth
			if (water)
			{
				PresetHelper_ReduceWater();
				smoothingPost = 2;
				currentInterface->smoothingAmount = 6;
				currentInterface->update = true;
				ApplyMaster();
			}
		}
		void Preset_Mountains(float landBias = 3, bool water = false) {
			CleanPreset();
			//perlin
			//frequency 5
			//amplitude 17
			//fractal 2
			//frefra 0.4
			faultingInterface.faultParams.faultingIterations = 50 * landBias;
			faultingInterface.faultParams.heightIncrease = 0.8f;
			faultingInterface.faultParams.negativeMultiplier = 0.8f;
			faultingInterface.seed = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 0;
			currentInterface->smoothingAmount = 1;
			currentInterface->update = true;
			ApplyPreview(2);
			ApplyMaster();
			historyInterface.AddHistoryKey(faultingInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 5.1f;
			perlinInterface.perlinParams.amplitude = 12;
			perlinInterface.perlinParams.fractal = 2;
			perlinInterface.perlinParams.frequencyMultiplier = 0.4;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 6;
			currentInterface->update = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			smoothingPost = 0;
			currentInterface->smoothingAmount = 2;
			currentInterface->update = true;
			noiseInterface.noiseParams.maxNoiseHeight = 5;
			heightBounds[0] = snowThreshold - 1;
			heightBounds[1] = 30;
			applyBounds = true;
			ApplyPreview(1);
			ApplyMaster();
			historyInterface.AddHistoryKey(noiseInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			if (water)
			{
				PresetHelper_ReduceWater();
			}

			applyBounds = false;


			//TM_InterfaceMASTER::AbsoluteHeightMap(tm[1]->changingHeightMap, terrainResolution);
		}

		void Preset_Mountains2(float landBias = 3, bool water = false) {

			CleanPreset();
			//Perlin Noise, smooth, ridge
			perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 0.4;
			perlinInterface.perlinParams.amplitude = 12 * landBias;
			perlinInterface.perlinParams.fractal = 1;
			perlinInterface.perlinParams.frequencyMultiplier = 2.0f;
			perlinInterface.perlinParams.amplitudeMultiplier = 0.5f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 0;
			currentInterface->smoothingAmount = 3;
			currentInterface->update = true;
			ridgeType = 1;
			ridgify = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));
			//Reverse the perlin noise direction
			ridgify = false;
			perlinInterface.perlinParams.xFrequency = 1.8f;
			perlinInterface.perlinParams.yFrequency = 4.8f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			currentInterface->update = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			//Reduce water and smooth
			if (water)
			{
				PresetHelper_ReduceWater();
				smoothingPost = 2;
				currentInterface->smoothingAmount = 6;
				currentInterface->update = true;
				ApplyMaster();
			}

			//Particle emitters on high areas
			particleDepositionInterface.pdParams.emitters.clear();
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			//Adds a dense positive
			particleDepositionInterface.pdParams.emitterDensity = 0.3f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = 0.1f;
			particleDepositionInterface.pdParams.heightParameters[1] = 0.5f;
			particleDepositionInterface.pdParams.iterations = 20;
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 8;
			applyBounds = true;
			heightBounds[0] = snowThreshold;
			heightBounds[1] = 20;
			currentInterface->update = true;
			ApplyPreview(3);
			ApplyMaster();
			historyInterface.AddHistoryKey(particleDepositionInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			particleDepositionInterface.pdParams.emitters.clear();
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			//Adds a dense positive
			particleDepositionInterface.pdParams.emitterDensity = 0.3f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = 0.1f;
			particleDepositionInterface.pdParams.heightParameters[1] = 0.4f;
			particleDepositionInterface.pdParams.iterations = 8;
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 2;
			applyBounds = true;
			heightBounds[0] = snowThreshold;
			heightBounds[1] = 20;
			currentInterface->update = true;
			ApplyPreview(3);
			ApplyMaster();
			historyInterface.AddHistoryKey(particleDepositionInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));


			perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 5.1f;
			perlinInterface.perlinParams.amplitude = 6.751;
			perlinInterface.perlinParams.fractal = 6;
			perlinInterface.perlinParams.frequencyMultiplier = 5.2f;
			perlinInterface.perlinParams.amplitudeMultiplier = 0.8f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 2;
			currentInterface->update = true;
			//ridgeType = 1;
			//ridgify = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));
		}

		void Preset_Valleys(float landBias = 3, bool water = false) {
			CleanPreset();
			//Perlin Noise, with smooth, ridge
			perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 0.4;
			perlinInterface.perlinParams.amplitude = 12;
			perlinInterface.perlinParams.fractal = 1;
			perlinInterface.perlinParams.frequencyMultiplier = 2.0f;
			perlinInterface.perlinParams.amplitudeMultiplier = 0.5f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 0;
			currentInterface->smoothingAmount = 3;
			currentInterface->update = true;
			ridgeType = 1;
			ridgify = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			//Reverse perlin direction
			ridgify = false;
			perlinInterface.perlinParams.xFrequency = 1.8f;
			perlinInterface.perlinParams.yFrequency = 4.8f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			currentInterface->update = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			if (water)
			{
				PresetHelper_ReduceWater();
				smoothingPost = 2;
				currentInterface->smoothingAmount = 6;
				currentInterface->update = true;
				ApplyMaster();
			}


			//Particle emitters at high areas, with a lot of iterations and smoothing
			particleDepositionInterface.pdParams.emitters.clear();
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			//Adds a sparse positive
			particleDepositionInterface.pdParams.emitterDensity = 0.30f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = 0.1f;
			particleDepositionInterface.pdParams.heightParameters[1] = 0.5f * landBias;
			particleDepositionInterface.pdParams.iterations = 81;
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 20;
			applyBounds = true;
			heightBounds[0] = snowThreshold;
			heightBounds[1] = 20;
			currentInterface->update = true;
			ApplyPreview(3);
			ApplyMaster();
			historyInterface.AddHistoryKey(particleDepositionInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			particleDepositionInterface.pdParams.emitters.clear();
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(0, 99999);
			srand(TM_InterfaceMASTER::seed);
			//Adds a dense positive
			particleDepositionInterface.pdParams.emitterDensity = 0.3f;
			particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			particleDepositionInterface.pdParams.heightParameters[0] = 0.1f;
			particleDepositionInterface.pdParams.heightParameters[1] = 0.4f;
			particleDepositionInterface.pdParams.iterations = 8;
			particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 2;
			applyBounds = true;
			heightBounds[0] = snowThreshold;
			heightBounds[1] = 20;
			currentInterface->update = true;
			ApplyPreview(3);
			ApplyMaster();
			historyInterface.AddHistoryKey(particleDepositionInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));


			perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 5.1f;
			perlinInterface.perlinParams.amplitude = 6.751;
			perlinInterface.perlinParams.fractal = 6;
			perlinInterface.perlinParams.frequencyMultiplier = 5.2f;
			perlinInterface.perlinParams.amplitudeMultiplier = 0.8f;
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedXDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.warpedYDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 1;
			currentInterface->smoothingAmount = 2;
			currentInterface->update = true;
			//ridgeType = 1;
			//ridgify = true;
			ApplyPreview(4);
			ApplyMaster();
			historyInterface.AddHistoryKey(perlinInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));
		}
		void Preset_Shallows(float landBias = 1.5f, bool water = false) {
			CleanPreset();
			//Perlin Noise, with smooth, ridge
			TM_InterfaceMASTER::seed = RandomBase10::EngineRandomNumber(1, 99999);
			srand(TM_InterfaceMASTER::seed);

			faultingInterface.faultParams.faultingIterations = RandomBase10::EngineRandomNumber(50, 300);
			faultingInterface.faultParams.smoothingFrequency = 2;
			faultingInterface.faultParams.smoothingStrength = 2;
			faultingInterface.faultParams.enableBottom = RandomBase10::EngineRandomNumber(0, 4);
			faultingInterface.faultParams.enableTop = RandomBase10::EngineRandomNumber(0, 4);;
			faultingInterface.faultParams.enableLeft = RandomBase10::EngineRandomNumber(0, 4);;
			faultingInterface.faultParams.enableRight = RandomBase10::EngineRandomNumber(0, 4);;
			faultingInterface.faultParams.heightIncrease = 0.5f;
			float negBias = -0.1 * landBias;
			faultingInterface.faultParams.negativeMultiplier = 1.1f + negBias;
			smoothingPost = 0;
			currentInterface->smoothingAmount = 3;
			currentInterface->update = true;
			ridgify = false;
			ApplyPreview(2);
			ApplyMaster();
			historyInterface.AddHistoryKey(faultingInterface.MakeHistoryKey(smoothingPost, currentInterface->smoothingAmount, ridgify, ridgeType, applyBounds, heightBounds[0], heightBounds[1]));

			if (water)
			{
				PresetHelper_ReduceWater();
				smoothingPost = 2;
				currentInterface->smoothingAmount = 6;
				currentInterface->update = true;
				ApplyMaster();
			}

		}
		void PresetHelper_ReduceWater() {
			ridgify = false;
			applyBounds = false;
			TM_InterfaceMASTER::NoZeroHeightMap(tm[1]->changingHeightMap, terrainResolution);
			/*perlinInterface.perlinParams.xFrequency = 5.1f;
			perlinInterface.perlinParams.yFrequency = 5.1f;
			perlinInterface.perlinParams.amplitude = 4;
			perlinInterface.perlinParams.fractal = 8;
			perlinInterface.perlinParams.frequencyMultiplier = 2.0f;
			perlinInterface.perlinParams.amplitudeMultiplier = 0.4f;
			perlinInterface.perlinParams.warping = true;
			perlinInterface.perlinParams.wXD = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.wYD = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.xDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			perlinInterface.perlinParams.yDisplacement = RandomBase10::EngineRandomNumber(0, 99999);
			smoothingPost = 0;
			currentInterface->smoothingAmount = 6;
			currentInterface->update = true;
			ApplyPreview(4);
			ApplyMaster();*/

			//applyBounds = true;
			//heightBounds[0] = -30;
			//heightBounds[1] = snowThreshold - 2;
			//particleDepositionInterface.pdParams.emitters.clear();
			//particleDepositionInterface.pdParams.seed = RandomBase10::EngineRandomNumber(0, 99999);
			//srand(particleDepositionInterface.pdParams.seed);
			////Adds a sparse positive
			//particleDepositionInterface.pdParams.emitterDensity = 0.3f;
			//particleDepositionInterface.pdParams.emitterRadius = 1.0f;
			//particleDepositionInterface.pdParams.emitterPoint[0] = 0.5;
			//particleDepositionInterface.pdParams.emitterPoint[1] = 0.5;
			//particleDepositionInterface.pdParams.heightParameters[0] = 0.1f;
			//particleDepositionInterface.pdParams.heightParameters[1] = 0.5f;
			//particleDepositionInterface.pdParams.iterations = 7;
			//particleDepositionInterface.AddEmitterPoints(terrainResolution, particleDepositionInterface.pdParams);
			//smoothingPost = 0;
			//currentInterface->smoothingAmount = 20;
			//currentInterface->update = true;
			//ApplyPreview(3);
			//ApplyMaster();
		}
		void PresetGUI() {
			static int extraParams = 0;
			static bool paramReset = false;
			static float f1 = 0.0f;
			static float f2 = 0.0f;
			static bool b1 = false;
			if (extraParams == 1)
			{
				if (paramReset)
				{
					f1 = 0.0f;
					f2 = 0.0f;
				}
				paramReset = false;
				TM_InterfaceMASTER::ValueChange("Land Increase", f1, 0.0f, 2.0f);
				TM_InterfaceMASTER::ValueChange("Swamp Decrease", f2, 0.0f, 2.0f);
			}
			if (extraParams == 2)
			{
				if (paramReset)
				{
					f1 = 1.0f;
					b1 = false;
				}
				paramReset = false;
				TM_InterfaceMASTER::ValueChange("Height Bias", f1, 0.5f, 3.0f);
				ImGui::Checkbox("Reduce Water", &b1);

			}
			if (ImGui::Button("Marsh Lands"))
			{
				if (extraParams != 1)
				{
					paramReset = true;

				}
				extraParams = 1;
				Preset_Marshlands(f1, f2);			
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
			if (ImGui::Button("Mountainous"))
			{
				if (extraParams != 2)
				{
					paramReset = true;

				}
				extraParams = 2;
				Preset_Mountains(f1, b1);
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
			if (ImGui::Button("Mountains"))
			{
				if (extraParams != 2)
				{
					paramReset = true;

				}
				extraParams = 2;
				Preset_Mountains2(f1, b1);
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
			if (ImGui::Button("Valleys"))
			{
				if (extraParams != 2)
				{
					paramReset = true;

				}
				extraParams = 2;
				Preset_Valleys(f1, b1);
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
			if (ImGui::Button("Shallows"))
			{
				if (extraParams != 2)
				{
					paramReset = true;

				}
				extraParams = 2;
				Preset_Shallows(f1, b1);
				regnerateAdditiveFunctions = true;
				currentInterface->update = true;
			}
		}
#pragma endregion

};

#endif // !TRIPLETERRAIN_H





//int my_image_width, my_image_height;
//ID3D11ShaderResourceView* my_texture = NULL;
//int once = 1;
//
//void ShowImage() {


//	if (ImGui::TreeNode("Images"))
//	{
//		if (once == 1)
//		{
//			once = 2;
//		}
//		ImGui::Image((void*)my_texture, ImVec2(my_image_width, my_image_height));
//		ImGui::TreePop();
//	}
//}
//void InImage(ID3D11Device* device) {
//	if (once == 2)
//	{
//		once = 0;
//		my_image_width = 0;
//		my_image_height = 0;
//		bool ret = LoadTextureFromFile(tm[0]->changingHeightMap, terrainResolution, &my_texture, &my_image_width, &my_image_height, device);
//	}
//}
// Simple helper function to load an image into a DX11 texture with common settings
//bool LoadTextureFromFile(float* hMap, int resolution, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* device)
//{
//	// Load from disk into a raw RGBA buffer
//	int image_width = resolution;
//	int image_height = resolution;
//	std::vector<DirectX::PackedVector::HALF> hmap;
//	for (int i = 0; i < resolution; i++)
//	{
//		for (int j = 0; j < resolution; j++)
//		{
//			hmap.push_back(DirectX::PackedVector::XMConvertFloatToHalf(hMap[(j * resolution) + i]));
//
//		}
//	}
//
//	// Create texture
//	D3D11_TEXTURE2D_DESC desc;
//	ZeroMemory(&desc, sizeof(desc));
//	desc.Width = image_width;
//	desc.Height = image_height;
//	desc.MipLevels = 1;
//	desc.ArraySize = 1;
//	desc.Format = DXGI_FORMAT_R16_FLOAT;
//	desc.SampleDesc.Count = 1;
//	desc.Usage = D3D11_USAGE_DEFAULT;
//	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	desc.CPUAccessFlags = 0;
//
//	ID3D11Texture2D* pTexture = NULL;
//	D3D11_SUBRESOURCE_DATA subResource;
//	subResource.pSysMem = hMap;
//	subResource.SysMemPitch = desc.Width * 4;
//	subResource.SysMemSlicePitch = 0;
//	device->CreateTexture2D(&desc, &subResource, &pTexture);
//
//	// Create texture view
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//	ZeroMemory(&srvDesc, sizeof(srvDesc));
//	srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = desc.MipLevels;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
//	pTexture->Release();
//
//	*out_width = image_width;
//	*out_height = image_height;
//
//	return true;
//}