#include "TM_InterfaceMASTER.h"
bool TM_InterfaceMASTER::update = false;
int TM_InterfaceMASTER::seed = 0;

void TM_InterfaceMASTER::SmoothGUI()
{
	ValueChange("Smoothing", smoothingAmount, 0, 50);

}

void TM_InterfaceMASTER::FlattenHeightMap(float* hMap, int resolution)
{
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			hMap[(j * resolution) + i] = 0;
		}
	}
}
void TM_InterfaceMASTER::AbsoluteHeightMap(float* hMap, int resolution)
{
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			hMap[(j * resolution) + i] = abs(hMap[(j * resolution) + i]);
		}
	}
}
void TM_InterfaceMASTER::NoZeroHeightMap(float* hMap, int resolution)
{
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			if (hMap[(j * resolution) + i] < 0)
			{
				hMap[(j * resolution) + i] = 0;
			}
		}
	}
}
void TM_InterfaceMASTER::RaiseToZeroHeightMap(float* hMap, int resolution)
{
	float lowestHeight = 0;
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			if (hMap[(j * resolution) + i] < lowestHeight)
			{
				lowestHeight = hMap[(j * resolution) + i];
			}
		}
	}
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			hMap[(j * resolution) + i] += abs(lowestHeight);
		}
	}
}

void TM_InterfaceMASTER::RidgeHeightMap(float* hMap, int resolution)
{
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
				hMap[(j * resolution) + i] = abs(hMap[(j * resolution) + i]) * -1;
		}
	}
	TM_InterfaceMASTER::RaiseToZeroHeightMap(hMap, resolution);

}

/// <summary>
/// Smooths the heightmap, with no weights. Using the surrounding 3x3 grid.
/// </summary>
void TM_InterfaceMASTER::SmoothHeightMap(float* hMap, int resolution, int smoothingAmount)
{
	if (smoothingAmount == 0)
		return;

	//The smoothing uses to literal height maps "hMap" & "newHeightMap" to store the proper values
	float* newHeightMap1 = new float[resolution * resolution];
	float* newHeightMap2 = new float[resolution * resolution];

	//Copies hMap into newHeightMap
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			newHeightMap1[(j * resolution) + i] = hMap[(j * resolution) + i];
			newHeightMap2[(j * resolution) + i] = hMap[(j * resolution) + i];
		}
	}


	//These two are for reference usage and allowing swaps between the two original height maps
	float* tempHeightMap1 = newHeightMap1;
	float* tempHeightMap2 = newHeightMap2;
	bool swap = 0;

	//HOW IT RUNS:
	//First iteration will have the newHeightMap average hMap
	//Second iterations will have the hMap average the newHeightMap(Which if you remember, averaged the hMap)
	//and so on so forth
	for (int smoothingIteraction = 0; smoothingIteraction < smoothingAmount; smoothingIteraction++)
	{

		for (int j = 0; j < (resolution); j++) {
			for (int i = 0; i < (resolution); i++) {

				float total = 0;
				//Below will just be checking if we have neighbours in certain directions. If we're on the perimeter, we will not have some neighbours.
				//Note: This just allows me to not use if statements. But everything below could be written as 'if' checks
				bool topCheck = ((j - 1) >= 0);
				bool bottomCheck = ((j + 1) < resolution);
				bool leftCheck = ((i - 1) >= 0);
				bool rightCheck = ((i + 1) < resolution);



				total += tempHeightMap2[((j * resolution) + i)];//Center

				total += tempHeightMap2[((j * resolution) + (i - 1)) * (leftCheck)] * (leftCheck);//Left
				total += tempHeightMap2[((j * resolution) + (i + 1)) * (rightCheck)] * (rightCheck);//Right

				total += tempHeightMap2[(((j - 1) * resolution) + i) * (topCheck)] * (topCheck);//Top
				total += tempHeightMap2[(((j - 1) * resolution) + (i - 1)) * (topCheck * leftCheck)] * (topCheck * leftCheck);//Top Left
				total += tempHeightMap2[(((j - 1) * resolution) + (i + 1)) * (topCheck * rightCheck)] * (topCheck * rightCheck);//Top Right

				total += tempHeightMap2[(((j + 1) * resolution) + i) * (bottomCheck)] * (bottomCheck);//Bottom
				total += tempHeightMap2[(((j + 1) * resolution) + (i - 1)) * (bottomCheck * leftCheck)] * (bottomCheck * leftCheck);//Bottom Left
				total += tempHeightMap2[(((j + 1) * resolution) + (i + 1)) * (bottomCheck * rightCheck)] * (bottomCheck * rightCheck);//Bottom Right


				int totalNeighbours = 9;//If we are anywhere but the perimeter
				//If we're only on the sides but not the corners
				if (!bottomCheck || !topCheck || !leftCheck || !rightCheck)
				{
					totalNeighbours = 6;
				}
				// If we are on corners
				if (((!topCheck || !bottomCheck) && !leftCheck) || ((!topCheck || !bottomCheck) && !rightCheck))
				{
					totalNeighbours = 4;
				}
				total /= totalNeighbours;
				tempHeightMap1[(j * resolution) + i] = total;
			}
		}

		//Switch around the tempHeightMaps to reuse above code
		if (!swap)
		{
			swap = 1;
			tempHeightMap1 = newHeightMap1;
			tempHeightMap2 = newHeightMap2;
		}
		else {
			swap = 0;
			tempHeightMap1 = newHeightMap2;
			tempHeightMap2 = newHeightMap1;
		}
	}

	tempHeightMap1 = nullptr;
	tempHeightMap2 = nullptr;

	//Checks what is our latest map that got smoothed. Deletes the other one.
	if ((smoothingAmount) % 2 == 1)
	{
		for (int j = 0; j < (resolution); j++) {
			for (int i = 0; i < (resolution); i++) {

				hMap[(j * resolution) + i] = newHeightMap2[(j * resolution) + i];
			}
		}
	}
	else {
		for (int j = 0; j < (resolution); j++) {
			for (int i = 0; i < (resolution); i++) {

				hMap[(j * resolution) + i] = newHeightMap1[(j * resolution) + i];
			}
		}
	}
	delete[] newHeightMap1;
	delete[] newHeightMap2;
	newHeightMap1 = nullptr;
	newHeightMap2 = nullptr;

}




void TM_InterfaceMASTER::ValueChange(const char* string, float& parameter, float minParameter, float maxParameter, bool disableWN, const char* format, float power)
{
	float prevValue = parameter;
	ImGui::SliderFloat(string, &parameter, minParameter, maxParameter, format, power);
	if (prevValue - parameter != 0)
		update = true;
}

void TM_InterfaceMASTER::ValueChange(const char* string, int& parameter, int minParameter, int maxParameter, bool disableWN, const char* format, float power)
{
	float prevValue = parameter;
	ImGui::SliderInt(string, &parameter, minParameter, maxParameter, format);
	if (prevValue - parameter != 0)
		update = true;
}

void TM_InterfaceMASTER::ValueChange2(const char* string, int* parameter, int minParameter, int maxParameter, bool disableWN, const char* format, float power)
{
	float prevValue = parameter[0];
	float prevValue2 = parameter[1];

	ImGui::SliderInt2(string, parameter, minParameter, maxParameter, format);
	if (prevValue - parameter[0] != 0 || prevValue2 - parameter[1] != 0)
		update = true;
}

void TM_InterfaceMASTER::ValueChange2(const char* string, float* parameter, float minParameter, float maxParameter, bool disableWN, const char* format, float power)
{
	float prevValue = parameter[0];
	float prevValue2 = parameter[1];

	ImGui::SliderFloat2(string, parameter, minParameter, maxParameter, format, power);
	if (prevValue - parameter[0] != 0 || prevValue2 - parameter[1] != 0)
		update = true;
}

void TM_InterfaceMASTER::RadioChange(const char* string, int* parameter, int num)
{
	int prevValue = *parameter;
	ImGui::RadioButton(string, parameter, num);
	if (prevValue - *parameter != 0)
		update = true;
}
