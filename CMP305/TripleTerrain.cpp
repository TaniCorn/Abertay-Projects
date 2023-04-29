#include "TripleTerrain.h"

#pragma region BASIC

TripleTerrain::TripleTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	currentInterface = &basicInterface;

	for (int i = 0; i < 3; i++)
	{
		tm[i] = new TerrainMesh(device, deviceContext, lresolution);
		tm[i]->Resize(lresolution);
		tm[i]->Regenerate(device, deviceContext);
		TM_InterfaceMASTER::FlattenHeightMap(tm[i]->changingHeightMap, lresolution);

		water[i] = new WaterTerrain(device, deviceContext, lresolution);
		water[i]->Resize(lresolution);
		water[i]->Regenerate(device, deviceContext);
		TM_InterfaceMASTER::FlattenHeightMap(water[i]->waterTerrain, lresolution);

		tmVisual[i] = true;
	}

	currentInterface->update = true;
	SwitchUIFunction(1);
}

//Cleanup the heightMap
TripleTerrain::~TripleTerrain() {
	delete[] tm;
	delete[] water;

}

void TripleTerrain::Resize(int newResolution) {

	for (int i = 0; i < 3; i++)
	{
		tm[i]->Resize(newResolution);
		TM_InterfaceMASTER::FlattenHeightMap(tm[i]->changingHeightMap, newResolution);		
		//water[i]->Resize(newResolution);
		//TM_InterfaceMASTER::FlattenHeightMap(water[i]->waterTerrain, newResolution);
	}
	terrainResolution = newResolution;

}
// Set up the heightmap and create or update the appropriate buffers
void TripleTerrain::Regenerate(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	//Calculate and store the height values into temporary heightmaps, preview and composite
	if (currentInterface->update && !(smoothingPost == 2 || (ridgify && ridgeType == 2))) {
		ApplyPreview(selectedFunction);
		ApplyComposite();
	}
	//If we have a master type on, display it on the preview map
	if (smoothingPost == 2 || (ridgify && ridgeType == 2))
	{
		PreviewMasterFunctions();
	}

	if (regnerateAdditiveFunctions) {
		ApplyMaster();
		for (int i = 0; i < 3; i++)
		{
			tm[i]->Regenerate(device, deviceContext);
			water[i]->Regenerate(device, deviceContext);
		}
	}
	else {
		tm[0]->Regenerate(device, deviceContext);
		water[0]->Regenerate(device, deviceContext);
		tm[2]->Regenerate(device, deviceContext);
		water[2]->Regenerate(device, deviceContext);
	}

	

}
#pragma endregion

#pragma region HEIGHTMAP_APPLICATION
void TripleTerrain::ApplyPreview(int funcNum) {
	switch (funcNum)
	{
	case 0:
		TM_InterfaceBASIC::ApplyFunction(terrainSize, terrainResolution, tm[0]->changingHeightMap, basicInterface.basicParams);
		break;
	case 1:
		TM_InterfaceNOISE::ApplyFunction(terrainSize, terrainResolution, tm[0]->changingHeightMap, noiseInterface.noiseParams);
		break;
	case 2:
		TM_InterfaceFAULTING::ApplyFunction(terrainSize, terrainResolution, tm[0]->changingHeightMap, faultingInterface.faultParams);
		break;
	case 3:
		TM_InterfacePARTICLEDEPOSITION::ApplyFunction(terrainResolution, tm[0]->changingHeightMap, particleDepositionInterface.pdParams);
		break;
	case 4:
		TM_InterfacePERLIN::FractalFunction(terrainResolution, tm[0]->changingHeightMap, perlinInterface.perlinParams);
		break;
	case 5:
		TM_InterfacePERLIN::WarpedFunction(terrainResolution, tm[0]->changingHeightMap, perlinInterface.perlinParams);
		break;
	default:
		break;
	}
	if (ridgeType == 0 && ridgify)
	{
		TM_InterfaceMASTER::RidgeHeightMap(tm[0]->changingHeightMap, terrainResolution);
	}
	if (smoothingPost == 0)
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[0]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);

	}

}

void TripleTerrain::ApplyComposite()
{

	//Apply master and preview together
	for (int i = 0; i < terrainResolution; i++)
	{
		for (int j = 0; j < terrainResolution; j++)
		{

			//Only add master and preview if bounds is off or if it is on, within bounds, else just copy master
			if ((tm[1]->changingHeightMap[(j * terrainResolution) + i] >= heightBounds[0] && tm[1]->changingHeightMap[(j * terrainResolution) + i] < heightBounds[1]) || !applyBounds)
				tm[2]->changingHeightMap[(j * terrainResolution) + i] = tm[0]->changingHeightMap[(j * terrainResolution) + i] + tm[1]->changingHeightMap[(j * terrainResolution) + i];
			else
				tm[2]->changingHeightMap[(j * terrainResolution) + i] = tm[1]->changingHeightMap[(j * terrainResolution) + i];
		}
	}
	//Apply post processes
	if (ridgeType == 1 && ridgify)
	{
		TM_InterfaceMASTER::RidgeHeightMap(tm[2]->changingHeightMap, terrainResolution);
	}
	if (smoothingPost == 1)
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[2]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);

	}
}

void TripleTerrain::ApplyMaster() {
	//Only updates master once
	regnerateAdditiveFunctions = false;

	//Master Processes take precedence
	if (smoothingPost == 2 && (ridgeType == 2 && ridgify))
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[1]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);
		TM_InterfaceMASTER::RidgeHeightMap(tm[1]->changingHeightMap, terrainResolution);
		return;
	}
	if (smoothingPost == 2)
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[1]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);
		return;
	}
	if (ridgeType == 2 && ridgify)
	{
		TM_InterfaceMASTER::RidgeHeightMap(tm[1]->changingHeightMap, terrainResolution);
		return;
	}

	//Add heightmap preview to master heightmap
	for (int i = 0; i < terrainResolution; i++)
	{
		for (int j = 0; j < terrainResolution; j++)
		{
			//Only apply if bounds is off or if it is on, within bounds
			if ((tm[1]->changingHeightMap[(j * terrainResolution) + i] >= heightBounds[0] && tm[1]->changingHeightMap[(j * terrainResolution) + i] < heightBounds[1]) || !applyBounds)
				tm[1]->changingHeightMap[(j * terrainResolution) + i] += tm[0]->changingHeightMap[(j * terrainResolution) + i];
		}
	}
	//Apply any post processes after
	if (ridgeType == 1 && ridgify)
	{
		TM_InterfaceMASTER::RidgeHeightMap(tm[1]->changingHeightMap, terrainResolution);
	}
	if (smoothingPost == 1)
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[1]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);

	}



}

void TripleTerrain::PreviewMasterFunctions()
{
	for (int i = 0; i < terrainResolution; i++)
	{
		for (int j = 0; j < terrainResolution; j++)
		{
			tm[0]->changingHeightMap[(j * terrainResolution) + i] = tm[1]->changingHeightMap[(j * terrainResolution) + i];
		}
	}
	if (smoothingPost == 2)
	{
		TM_InterfaceMASTER::SmoothHeightMap(tm[0]->changingHeightMap, terrainResolution, currentInterface->smoothingAmount);

	}
	if (ridgeType == 2 && ridgify)
	{
		TM_InterfaceMASTER::RidgeHeightMap(tm[0]->changingHeightMap, terrainResolution);

	}
}

void TripleTerrain::UseHistory() {
	TM_InterfaceMASTER::FlattenHeightMap(tm[1]->changingHeightMap, terrainResolution);
	std::queue<HistoryKey> history = historyInterface.GetQueueHistory();
	int size = history.size();
	for (int i = 0; i < size; i++)
	{
		HistoryKey h = history.front();
		history.pop();
		smoothingPost = h.smoothingPost;
		currentInterface->smoothingAmount = h.smoothAmount;

		if (h.function == "BASIC")
		{
			basicInterface.UseHistoryKey(h);
			ApplyPreview(0);
			ApplyMaster();
		}
		else if (h.function == "NOISE") {
			noiseInterface.UseHistoryKey(h);
			ApplyPreview(1);
			ApplyMaster();

		}
		else if (h.function == "FAULTING") {

			faultingInterface.UseHistoryKey(h);
			ApplyPreview(2);
			ApplyMaster();
		}
		else if (h.function == "PARTICLEDEPOSITION") {
			particleDepositionInterface.pdParams.emitters.clear();
			particleDepositionInterface.UseHistoryKey(h);
			ApplyPreview(3);
			ApplyMaster();

		}
		else if (h.function == "PERLIN" && std::stoi(h.parameters[0]) == 0) {
			perlinInterface.UseHistoryKey(h);
			ApplyPreview(4);
			ApplyMaster();

		}
		else if (h.function == "PERLIN" && std::stoi(h.parameters[0]) == 1) {
			perlinInterface.UseHistoryKey(h);
			ApplyPreview(5);
			ApplyMaster();

		}

	}
	currentInterface->update = true;
}


#pragma endregion