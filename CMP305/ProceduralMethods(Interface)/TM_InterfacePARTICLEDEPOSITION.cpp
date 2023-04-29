#include "TM_InterfacePARTICLEDEPOSITION.h"

int TM_InterfacePARTICLEDEPOSITION::stackOverflow = 0;
//Local value changes have been made so we do not trigger the update variable
void LocalValueChange(bool* change, const char* string, float& parameter, float minParameter, float maxParameter)
{
	float prevValue = parameter;
	ImGui::SliderFloat(string, &parameter, minParameter, maxParameter);
	if (prevValue - parameter != 0)
		*change = true;
}
void LocalValueChange2(bool* change, const char* string, float* parameter, float minParameter, float maxParameter)
{
	float prevValue = parameter[0];
	float prevValue2 = parameter[1];

	ImGui::SliderFloat2(string, parameter, minParameter, maxParameter);
	if (prevValue - parameter[0] != 0 || prevValue2 - parameter[1] != 0)
		*change = true;
}

void TM_InterfacePARTICLEDEPOSITION::HandleGUI()
{
	bool change = false;
	ImGui::TextWrapped("GUIDE: The buttons will spawn in particles automatically. The manual emitter parameters allow you to see a circle on the preview and composite heightmaps, and you can place particles manually.");
	ImGui::TextWrapped("After placing your particles, you must control how many iterations there are.");
	PresetButtons();

	ValueChange("Seed", TM_InterfaceMASTER::seed, 0, 100);
	ImGui::Text("Simulation Parameters");
	if (ImGui::Button("Delete all emitters"))
	{
		pdParams.emitters.clear();
		change = true;
	}
	if (change == true)
	{
		update = true;
		pdParams.iterations = 0;
	}
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Make sure you have put some particles on the map first");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Negative particles may cause massive slowness");
	ValueChange("Iterations", pdParams.iterations, 0, 1000);
	ValueChange("Height Threshold", pdParams.recursiveHeightThreshold, 0, 10); Tooltip("The height difference neighbours must be at to displace particles");
	ImGui::Checkbox("Recursive Particle Deposition", &pdParams.recursive); Tooltip("Instead of checking only the neighbours, this will recursively check neighbours of neighbours and so on");
	ImGui::Checkbox("Fuzziness", &pdParams.fuzz); Tooltip("Particle Emitters may not emit at their exact location");
	ValueChange("Fuzz Radius", pdParams.fuzzRadius, 3, 100); Tooltip("The radius that the fuzz can be");
	
	ImGui::Text("Particle Parameters");
	if (ImGui::TreeNode("Manual Emitter Parameters"))
	{
		ImGui::Text("Emitter Parameters");

		LocalValueChange2(&change, "Point to emit", pdParams.emitterPoint, 0.0f, 0.98f);
			LocalValueChange(&change, "Emitter Radius", pdParams.emitterRadius, 0.0f, 1.0f);
			LocalValueChange(&change, "Emitter Density", pdParams.emitterDensity, 0.0f, 1.0f);
			LocalValueChange2(&change, "Min - Max Height of particle", pdParams.heightParameters, -10, 10);

			if (ImGui::Button("Save Emitter Location")) {
				pdParams.settingPoint = true;
				update = true;
			}
			ImGui::TreePop();
			if (change == true)
			{
				update = true;
			}
	}

}

void TM_InterfacePARTICLEDEPOSITION::MarshLandsParameters()
{
	TM_InterfaceMASTER::seed = rand();
	pdParams.emitterDensity = 0.02f;
	pdParams.emitterRadius = 1.0f;
	pdParams.emitterPoint[0] = 0.5;
	pdParams.emitterPoint[1] = 0.5;
	pdParams.heightParameters[0] = 0.1f;
	pdParams.heightParameters[1] = 0.5f;
}

void TM_InterfacePARTICLEDEPOSITION::PresetButtons()
{
	if (ImGui::Button("Spawn Dense Positive Particles")) {
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 0.3;
		pdParams.emitterRadius = 1.0f;
		pdParams.emitterPoint[0] = 0.5;
		pdParams.emitterPoint[1] = 0.5;
		pdParams.heightParameters[0] = 0.1f;
		pdParams.heightParameters[1] = 0.5f;
		pdParams.settingPoint = true;
		update = true;
	}
	if (ImGui::Button("Spawn Sparse Positive Particles")) {
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 0.02f;
		pdParams.emitterRadius = 1.0f;
		pdParams.emitterPoint[0] = 0.5;
		pdParams.emitterPoint[1] = 0.5;
		pdParams.heightParameters[0] = 0.1f;
		pdParams.heightParameters[1] = 0.5f;
		pdParams.settingPoint = true;
		update = true;
	}	
	if (ImGui::Button("Spawn Dense Negative Particles")) {
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 0.3;
		pdParams.emitterRadius = 0.8f;
		pdParams.emitterPoint[0] = 0.5;
		pdParams.emitterPoint[1] = 0.5;
		pdParams.heightParameters[0] = -0.1f;
		pdParams.heightParameters[1] = -0.5f;
		pdParams.settingPoint = true;
		update = true;
	}
	if (ImGui::Button("Spawn Sparse Negative Particles")) {
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 0.02f;
		pdParams.emitterRadius = 1.0f;
		pdParams.emitterPoint[0] = 0.5;
		pdParams.emitterPoint[1] = 0.5;
		pdParams.heightParameters[0] = -0.1f;
		pdParams.heightParameters[1] = -0.5f;
		pdParams.settingPoint = true;
		update = true;
	}
	if (ImGui::Button("Spawn Minimal Random Positive Particle"))
	{
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 1.f;
		pdParams.emitterRadius = 0.01f;
		pdParams.emitterPoint[0] = (float(rand() %100)/100.0f);
		pdParams.emitterPoint[1] = (float(rand() % 100) / 100.0f);
		pdParams.heightParameters[0] = 0.1f;
		pdParams.heightParameters[1] = 0.5f;
		pdParams.settingPoint = true;
		update = true;
	}	
	if (ImGui::Button("Spawn Minimal Random Negative Particle"))
	{
		TM_InterfaceMASTER::seed = rand();
		pdParams.emitterDensity = 1.f;
		pdParams.emitterRadius = 0.01f;
		pdParams.emitterPoint[0] = (float(rand() %100)/100.0f);
		pdParams.emitterPoint[1] = (float(rand() % 100) / 100.0f);
		pdParams.heightParameters[0] = 0.1f;
		pdParams.heightParameters[1] = 0.5f;
		pdParams.settingPoint = true;
		update = true;
	}
}



#pragma region PARTICLE_DEPOSITION
void TM_InterfacePARTICLEDEPOSITION::ApplyFunction(int resolution, float* hMap, ParticleDepositionParams& params)
{
	//WARNING: Conversions from float to int and vice versa can cause very innacurrate numbers
	//126.0f can be converted to 133
	//Check every int before accessing arrays
	TM_InterfaceMASTER::FlattenHeightMap(hMap, resolution);
	srand(TM_InterfaceMASTER::seed);


	//Manual placement view shows when iterations is 0
	if (params.iterations == 0)
	{
		UserDefinedPoints(resolution, hMap, params);
		return;
	}
	bool negativeVals = false;
	for each (EmitPoint2 var in params.emitters)
	{
		if (var.heightParameters[0] < 0 || var.heightParameters[1] < 0)
		{
			negativeVals = true;
			break;
		}
	}
	if (negativeVals)
	{
		for (int iter = 0; iter < params.iterations; iter++)
		{
			//Go through all the saved particles
			for each (EmitPoint2 var in params.emitters)
			{
				stackOverflow = 0;
				float height;
				int randomX = 0, randomY = 0;
				if (params.fuzz)
				{
					randomX = rand() % params.fuzzRadius;
					randomY = rand() % params.fuzzRadius;
					randomX -= params.fuzzRadius / 2;
					randomY -= params.fuzzRadius / 2;
				}

				XMINT2 savedEmitterPoint = XMINT2(var.x + randomX, var.y + randomY);

				//SCRAP:When positive particles are present, this is just a tiny bit faster than simply doing negative. That's how much slower Abs() is.
				//Real reason, now that we're doing the negative threshold and reversing the comparisons. We need this check to make sure that we do not influence any positive particles.
				if (var.heightParameters[0] < 0 || var.heightParameters[1] < 0)
				{
					savedEmitterPoint = CheckNeighboursMooreParticleDeposition_Negative(resolution, hMap, savedEmitterPoint, params.recursiveHeightThreshold, params.recursive);//Grabs a random neighbour to increase the height for
				}
				else {

					savedEmitterPoint = CheckNeighboursMooreParticleDeposition(resolution, hMap, savedEmitterPoint, params.recursiveHeightThreshold, params.recursive);//Grabs a random neighbour to increase the height for
				}

				//Getting the random height
				if ((var.heightParameters[1] - var.heightParameters[0]) <= 1 && (var.heightParameters[0] - var.heightParameters[1]) >= -1)//If the height min max difference is small, just set it to the max
					height = int(var.heightParameters[1] * 100);
				else
					height = ((rand() % (int(var.heightParameters[1] * 100) - int(var.heightParameters[0] * 100)) + int(var.heightParameters[0] * 100)));//These silly math conversions are purely for the rand function

				height /= 100.0f;


				int index = (savedEmitterPoint.y * resolution) + savedEmitterPoint.x;
				if (index < 0 || index > resolution * resolution)//ARRAY SAFEGUARD
					continue;
				hMap[index] += height;//Height increase
			}

		}
	}
	else {
		for (int iter = 0; iter < params.iterations; iter++)
		{
			//Go through all the saved particles
			for each (EmitPoint2 var in params.emitters)
			{
				stackOverflow = 0;
				float height;
				int randomX = 0, randomY = 0;
				if (params.fuzz)
				{
					randomX = rand() % params.fuzzRadius;
					randomY = rand() % params.fuzzRadius;
					randomX -= params.fuzzRadius / 2;
					randomY -= params.fuzzRadius / 2;
				}

				XMINT2 savedEmitterPoint = XMINT2(var.x + randomX, var.y + randomY);
				savedEmitterPoint = CheckNeighboursMooreParticleDeposition(resolution, hMap, savedEmitterPoint, params.recursiveHeightThreshold, params.recursive);//Grabs a random neighbour to increase the height for

				//Getting the random height
				if ((var.heightParameters[1] - var.heightParameters[0]) <= 1 && (var.heightParameters[0] - var.heightParameters[1]) >= -1)//If the height min max difference is small, just set it to the max
					height = int(var.heightParameters[1] * 100);
				else
					height = ((rand() % (int(var.heightParameters[1] * 100) - int(var.heightParameters[0] * 100)) + int(var.heightParameters[0] * 100)));//These silly math conversions are purely for the rand function

				height /= 100.0f;


				int index = (savedEmitterPoint.y * resolution) + savedEmitterPoint.x;
				if (index < 0 || index > resolution * resolution)//ARRAY SAFEGUARD
					continue;
				hMap[index] += height;//Height increase
			}

		}
	}
	


}

#pragma region USERDEFINEDPOINTS
void TM_InterfacePARTICLEDEPOSITION::UserDefinedPoints(int resolution, float* hMap, ParticleDepositionParams& params)
{
	////////////Current Selection
	int p1 = (params.emitterPoint[0]) * resolution;
	int p2 = (params.emitterPoint[1]) * resolution;
	float radius = (params.emitterRadius) * resolution;
	//Note to self, for some reason doing emitterRadius * resolution//will cause a crash of heap error. Unknown reason why but this current implmentation doesn't crash

	//Display Current Point + Circle radius
	DisplayCurrentPoint(resolution, hMap, p1, p2, radius, params.heightParameters[1]);

	//Set random points in circle
	if (params.settingPoint)
	{
		params.settingPoint = false;

		AddEmitterPoints(resolution, params);
	}


	XMFLOAT2 savedEmiiterPoint;
	//Display Saved Positive Points by raising height of points
	for each (EmitPoint2 var in params.emitters)
	{
		savedEmiiterPoint = XMFLOAT2(var.x, var.y);
		int index = (savedEmiiterPoint.y * resolution) + savedEmiiterPoint.x;
		//ARRAY SAFEGUARD
		if (index < 0 || index > resolution * resolution)
			continue;

		hMap[index] += var.heightParameters[1];
	}


}
void TM_InterfacePARTICLEDEPOSITION::AddEmitterPoints(int resolution, ParticleDepositionParams& params)
{
	//Grabs points in a circle
	int p1 = (params.emitterPoint[0]) * resolution;
	int p2 = (params.emitterPoint[1]) * resolution;
	float radius = (params.emitterRadius) * resolution;
	std::vector<XMFLOAT2> newPoints = RandomisePointsInCircle(resolution, XMINT2(p1, p2), radius, params.emitterDensity);

	//Go over all points and adds them to the emitters
	for (int i = 0; i < newPoints.size(); i++)
	{
		EmitPoint2 point = { newPoints[i].x, newPoints[i].y, params.heightParameters[0], params.heightParameters[1] };
		//This is the only way I've found to safely insert sets of points into a std::set of a struct type
		auto ptr = params.emitters.insert(point).first;
		ptr._Ptr->_Myval.heightParameters[0] = point.heightParameters[0];
		ptr._Ptr->_Myval.heightParameters[1] = point.heightParameters[1];
	}
}

void TM_InterfacePARTICLEDEPOSITION::DisplayCurrentPoint(int resolution, float* hMap, float xP1, float zP1, float radius, float visualHeightIncrease)
{
	//ARRAY SAFEGUARD, makes sure we don't go out of bounds
	XMINT2 emiiterPoint = XMINT2(xP1, zP1);
	if (emiiterPoint.x < 0 || emiiterPoint.x > resolution || emiiterPoint.y < 0 || emiiterPoint.y > resolution)
	{
		emiiterPoint = XMINT2(0, 0);
	}


	//Display a circle around point
	hMap[(emiiterPoint.y * resolution) + emiiterPoint.x] += visualHeightIncrease;//Displays center of circle
	for (int i = 0; i <= 360; i += 3)
	{
		//Generates a raised circle around the origin point to visualise
		int z = (cos(i) * (radius));
		int x = (sin(i) * (radius));

		//Below is generating the points around the circle, imagine the circle split into 4 quadrants and this is going over those quadrants
		int index = (((emiiterPoint.y + z) * resolution) + emiiterPoint.x + x);
		if (index >= 0 && index < resolution * resolution)
			hMap[index] += visualHeightIncrease;

		//index = (((emiiterPoint.y - z) * resolution) + emiiterPoint.x - x);
		//if (index >= 0 && index < resolution * resolution)
		//	hMap[index] += visualHeightIncrease;

		//index = (((emiiterPoint.y - z) * resolution) + emiiterPoint.x + x);
		//if (index >= 0 && index < resolution * resolution)
		//	hMap[index] += visualHeightIncrease;

		//index = (((emiiterPoint.y + z) * resolution) + emiiterPoint.x - x);
		//if (index >= 0 && index < resolution * resolution)
		//	hMap[index] += visualHeightIncrease;

	}

}

#include "TS_MathHelper.h"

/// <summary>
/// Grabs random points inside a circle
/// </summary>
std::vector<XMFLOAT2> TM_InterfacePARTICLEDEPOSITION::RandomisePointsInCircle(int resolution, XMINT2 circleOrigin, float circleRadius, float emitterDensity)
{
	std::vector<XMFLOAT2> possibleEmitterPoints;

	//Todo: There is probably a more efficient way of going about this
	//Collision Detection for all possible array points. Adds any in circle to the possibleEmitterPoints
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			//This is just grabbing the hypotenuse of a triangle from circle center to end point and comparing it to the circleRadius
			XMFLOAT3 diff = XMFLOAT3(circleOrigin.x - i, 0, circleOrigin.y - j);
			float diffMag = TS_MATHS::TS_MATHS_DirectX::MagnitudeSqrRoot(diff);
			if ((diffMag * resolution) < (circleRadius * resolution))
				possibleEmitterPoints.push_back(XMFLOAT2(i, j));
		}
	}

	if (possibleEmitterPoints.size() == 0)
		return possibleEmitterPoints;

	//Chooses random points in the possible list with a density amount to add to the final points for emitters
	std::vector<XMFLOAT2> confirmedEmitterPoints;
	int iTotal = possibleEmitterPoints.size() * emitterDensity;//Density is between 0 and 1
	for (int i = 0; i < iTotal; i++)
	{
		int index = rand() % possibleEmitterPoints.size();

		confirmedEmitterPoints.push_back(possibleEmitterPoints[index]);
		possibleEmitterPoints.erase(possibleEmitterPoints.begin() + index);
	}

	return confirmedEmitterPoints;

}
#pragma endregion

XMINT2 TM_InterfacePARTICLEDEPOSITION::CheckNeighboursMooreParticleDeposition(int resolution, float* hMap, XMINT2 originalPosition, float threshold, bool recursive)
{
	stackOverflow++;
	if (stackOverflow > 100)
	{
		return originalPosition;
	}
	//This is just here for cleaner code
	int j = originalPosition.y;
	int i = originalPosition.x;
	std::vector<XMINT2> possibleLocations;
	if (j < 0 || j > resolution || i < 0 || i > resolution)//ARRAY SAFEGUARD
		return XMINT2(9999, 9999);

	const float originalHeight = hMap[((j * resolution) + i)];

	//This is to allow corners and sides to be considered efficiently without if statements
	bool topCheck = ((j - 1) >= 0);
	bool bottomCheck = ((j + 1) < resolution);
	bool leftCheck = ((i - 1) >= 0);
	bool rightCheck = ((i + 1) < resolution);

	//Check all 8 neighbours and origin
	float valueCheck;
	possibleLocations.push_back(XMINT2(i, j));

#pragma region Neighbours
	valueCheck = (originalHeight - hMap[((j * resolution) + (i - 1)) * (leftCheck)]) * (leftCheck);//Left
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i-1, j));

	valueCheck = (originalHeight - hMap[((j * resolution) + (i + 1)) * (rightCheck)]) * (rightCheck);//Right
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i+1,j));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + i) * (topCheck)]) * (topCheck);//Top
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i, j-1));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + (i - 1)) * (topCheck * leftCheck)]) * (topCheck * leftCheck);//Top Left
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i-1,j-1));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + (i + 1)) * (topCheck * rightCheck)]) * (topCheck * rightCheck);//Top Right
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i+1,j-1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + i) * (bottomCheck)]) * (bottomCheck);//Bottom
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i,j+1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + (i - 1)) * (bottomCheck * leftCheck)]) * (bottomCheck * leftCheck);//Bottom Left
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i-1,j+1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + (i + 1)) * (bottomCheck * rightCheck)]) * (bottomCheck * rightCheck);//Bottom Right
	if (valueCheck > threshold)
		possibleLocations.push_back(XMINT2(i+1,j+1));
#pragma endregion


	//We still need to check the x and y values or there are possible errors, despite the boolean checks
	for (int index = 0; index < possibleLocations.size(); index++)
	{
		if (possibleLocations[index].x < 0 || possibleLocations[index].x > resolution)
		{
			leftCheck = 1;
		}
		if (possibleLocations[index].y < 0 || possibleLocations[index].y > resolution)
		{
			leftCheck = 1;

		}
	}
	//WARNING: For an unknown reason this function can produce indexs that are outside the normal range

	//Grabs a random neighbour or self to increase height of
	if (possibleLocations.size() > 1 && recursive)
	{
		possibleLocations.erase(possibleLocations.begin());
		//return possibleLocations[(rand() % possibleLocations.size())];

		return CheckNeighboursMooreParticleDeposition(resolution, hMap, possibleLocations[(rand() % possibleLocations.size())], threshold, recursive);
	}
	else {
 		return possibleLocations[(rand() % possibleLocations.size())];
	}
}
XMINT2 TM_InterfacePARTICLEDEPOSITION::CheckNeighboursMooreParticleDeposition_Negative(int resolution, float* hMap, XMINT2 originalPosition, float threshold, bool recursive)
{
	stackOverflow++;
	if (stackOverflow > 100)
	{
		return originalPosition;
	}
	//This is just here for cleaner code
	int j = originalPosition.y;
	int i = originalPosition.x;
	std::vector<XMINT2> possibleLocations;
	if (j < 0 || j > resolution || i < 0 || i > resolution)//ARRAY SAFEGUARD
		return XMINT2(9999, 9999);

	const float originalHeight = hMap[((j * resolution) + i)];

	//This is to allow corners and sides to be considered efficiently without if statements
	bool topCheck = ((j - 1) >= 0);
	bool bottomCheck = ((j + 1) < resolution);
	bool leftCheck = ((i - 1) >= 0);
	bool rightCheck = ((i + 1) < resolution);

	//Check all 8 neighbours and origin
	float valueCheck;
	possibleLocations.push_back(XMINT2(i, j));
	
	//I've tested a double comparison with a negative, abs other things, fancy maths but this is just the fastest
	threshold *= -1;
#pragma region Neighbours
	valueCheck = (originalHeight - hMap[((j * resolution) + (i - 1)) * (leftCheck)]) * (leftCheck);//Left
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i - 1, j));

	valueCheck = (originalHeight - hMap[((j * resolution) + (i + 1)) * (rightCheck)]) * (rightCheck);//Right
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i + 1, j));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + i) * (topCheck)]) * (topCheck);//Top
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i, j - 1));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + (i - 1)) * (topCheck * leftCheck)]) * (topCheck * leftCheck);//Top Left
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i - 1, j - 1));

	valueCheck = (originalHeight - hMap[(((j - 1) * resolution) + (i + 1)) * (topCheck * rightCheck)]) * (topCheck * rightCheck);//Top Right
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i + 1, j - 1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + i) * (bottomCheck)]) * (bottomCheck);//Bottom
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i, j + 1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + (i - 1)) * (bottomCheck * leftCheck)]) * (bottomCheck * leftCheck);//Bottom Left
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i - 1, j + 1));

	valueCheck = (originalHeight - hMap[(((j + 1) * resolution) + (i + 1)) * (bottomCheck * rightCheck)]) * (bottomCheck * rightCheck);//Bottom Right
	if ((valueCheck) < threshold)
		possibleLocations.push_back(XMINT2(i + 1, j + 1));
#pragma endregion


	for (int index = 0; index < possibleLocations.size(); index++)
	{
		if (possibleLocations[index].x < 0 || possibleLocations[index].x > resolution)
		{
			leftCheck = 1;
		}
		if (possibleLocations[index].y < 0 || possibleLocations[index].y > resolution)
		{
			leftCheck = 1;

		}
	}
	//WARNING: For an unknown reason this function can produce indexs that are outside the normal range

	//Grabs a random neighbour or self to increase height of
	if (possibleLocations.size() > 1 && recursive)
	{
		possibleLocations.erase(possibleLocations.begin());
		//return possibleLocations[(rand() % possibleLocations.size())];

		return CheckNeighboursMooreParticleDeposition_Negative(resolution, hMap, possibleLocations[(rand() % possibleLocations.size())], threshold, recursive);
	}
	else {
		return possibleLocations[(rand() % possibleLocations.size())];
	}
}
#pragma endregion
