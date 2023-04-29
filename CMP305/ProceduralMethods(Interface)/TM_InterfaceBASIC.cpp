#include "TM_InterfaceBASIC.h"


void TM_InterfaceBASIC::HandleGUI()
{

	ValueChange2("Amplitude", basicParams.amplitude, 0.001f, 10.0f);
	ValueChange2("Frequency", basicParams.frequency, 0.0f, 2.0f);
	ValueChange2("Displacement", basicParams.displacement, 0.0f, 10.00f);

	ImGui::Checkbox("Auto Scroll X", &autoScrollX);
	if (autoScrollX) {
		update = true;
		basicParams.displacement[0] += 0.05f;
	}
	ImGui::Checkbox("Auto Scroll Z", &autoScrollZ);
	if (autoScrollZ) {
		update = true;
		basicParams.displacement[1] += 0.05f;
	}

}

void TM_InterfaceBASIC::ApplyFunction(float terrainSize, int resolution, float* hMap, WaveParams waves) {
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;
	const float Xfrequency = waves.frequency[0];
	const float xDisplacement = waves.displacement[0];
	const float XAmplitude = waves.amplitude[0];

	const float Zfrequency = waves.frequency[1];
	const float ZDispolacemnet = waves.displacement[1];
	const float ZAmplitude = waves.amplitude[1];

	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			height = sin((float)i * Xfrequency * scale + xDisplacement) * XAmplitude * 1.f;
			height += (cos((float)i * Xfrequency * 2 * scale + xDisplacement) * XAmplitude * 0.3f);

			height += sin((float)j * Zfrequency * scale + xDisplacement) * XAmplitude * 1.f;
			height += (cos((float)j * scale * Zfrequency + ZDispolacemnet)) * ZAmplitude;

			hMap[(j * resolution) + i] = height;
		}
	}
}
