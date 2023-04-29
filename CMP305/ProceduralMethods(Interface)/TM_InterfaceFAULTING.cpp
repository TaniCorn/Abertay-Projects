#include "TM_InterfaceFAULTING.h"
void TM_InterfaceFAULTING::HandleGUI()
{
	ValueChange("Seed", TM_InterfaceMASTER::seed, 0, 10);
	ValueChange("Faulting Iterations", faultParams.faultingIterations, 0, 500);
	ValueChange("Height Increase per Fault", faultParams.heightIncrease, 0.0f, 5.0f); Tooltip("Controls both negative and positive height increase, see'Height Bias' to reduce negative height");
	ValueChange("Height Bias", faultParams.negativeMultiplier, 0.6f, 1.4f); Tooltip("Controls the height decrease strength");

	ImGui::Checkbox("Enable Lines to Bottom", &faultParams.enableBottom);
	ImGui::Checkbox("Enable Lines to Right", &faultParams.enableRight);
	ImGui::Checkbox("Enable Lines to Top", &faultParams.enableTop);
	ImGui::Checkbox("Enable Lines to Left", &faultParams.enableLeft);

	ValueChange("Smoothing Frequency", faultParams.smoothingFrequency, 0, 100); Tooltip("Percentage of how many smooths happen per x faults");
	ValueChange("Smoothing Strength", faultParams.smoothingStrength, 0, 100);
	

}



#pragma region FAULTING


void TM_InterfaceFAULTING::ApplyFunction(float terrainSize, int resolution, float* hMap, FaultingParams params)
{

	//If there's only one direction enabled, return
	if ((params.enableBottom + params.enableRight + params.enableTop + params.enableLeft) <= 1)
		return;

	const float scale = terrainSize / (float)resolution;
	srand(TM_InterfaceMASTER::seed);
	TM_InterfaceMASTER::FlattenHeightMap(hMap, resolution);

	//Parameters setup
	std::vector<int> directionsAvailable;
	if (params.enableLeft)
		directionsAvailable.push_back(0);
	if (params.enableRight)
		directionsAvailable.push_back(1);
	if (params.enableTop)
		directionsAvailable.push_back(2);
	if (params.enableBottom)
		directionsAvailable.push_back(3);	


	int smoothingWait = -1;
	//Smoothing wait changes the parameter to be a percentage based off faulting iterations.
	//For example, if faulting iterations is 100, smoothing frequency is 1. It will only smooth once, at the end)
	//For example, if faulting iterations is 100, smoothing frequency is 50. It will smooth every 2 faults.
	if (params.smoothingFrequency != 0)
		smoothingWait = params.faultingIterations/ (params.faultingIterations * (float(params.smoothingFrequency)/100.0f));

	//Faulting with(/out) intermittent smoothing
	for (int iterations = 0; iterations < params.faultingIterations; iterations++)
	{
		Faulting(resolution, hMap, directionsAvailable, params);
		if (smoothingWait == 0)
		{
			SmoothHeightMap(hMap,resolution, params.smoothingStrength);
			smoothingWait = params.faultingIterations / (params.faultingIterations * (float(params.smoothingFrequency) / 100.0f));
		}
		smoothingWait--;


	}

}

#include "TS_MathHelper.h"

/// <summary>
/// One Fault Line
/// </summary>
void TM_InterfaceFAULTING::Faulting(int resolution, float* hMap, std::vector<int> copy_directionsAvailable, FaultingParams params)
{
	//Grab two points at the edge of the heightmap with the directions available
	XMFLOAT3 p1 = ChooseEdgePoint(resolution, copy_directionsAvailable);
	XMFLOAT3 p2 = ChooseEdgePoint(resolution, copy_directionsAvailable);

	
	XMFLOAT3 faultLineVector = XMFLOAT3(p2.x - p1.x, 0, p2.z - p1.z);
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			XMFLOAT3 currentVector = XMFLOAT3(p1.x - i, 0, p1.z - j);//Grabbing vector from p1 to current heightmap point
			//If the angle between the two vectors are between 0 and 180 degrees. Y will be positive. If it's between 180 and 360, Y will be negative.
			XMFLOAT3 result = TS_MATHS::TS_MATHS_DirectX::CrossProduct(faultLineVector, currentVector);

			if (result.y > 0)
				hMap[(j * resolution) + i] += params.heightIncrease;
			else
				hMap[(j * resolution) + i] -= params.heightIncrease * params.negativeMultiplier;

		}
	}
}
///Given a a set of directions, chooses a random point along one of them
XMFLOAT3 TM_InterfaceFAULTING::ChooseEdgePoint(int resolution, std::vector<int> directionsAvailable)
{
	//Chooses a random point along resolution
	int randp1 = (rand() % resolution);

	//Chooses a random direction
	int it = (rand() % (directionsAvailable.size()));
	int direction = directionsAvailable[it];
	directionsAvailable.erase(directionsAvailable.begin() + it);//Deletes the direction chosen, so we don't use it again for the second point

	switch (direction)
	{
	case 0:
		return XMFLOAT3(0, 0, randp1);//Left
		break;
	case 1:
		return XMFLOAT3(resolution, 0, randp1);//Right
		break;
	case 2:
		return XMFLOAT3(randp1, 0, resolution);//Top
		break;
	case 3:
		return XMFLOAT3(randp1, 0, 0);//Bottom
		break;
	default:
		break;
	}

}

#pragma endregion
