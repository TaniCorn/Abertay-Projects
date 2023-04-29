#include "TM_InterfaceNOISE.h"

void TM_InterfaceNOISE::HandleGUI()
{
	ValueChange("Noise Seed", TM_InterfaceMASTER::seed, 0, 100);
	ValueChange("Max Noise Height", noiseParams.maxNoiseHeight, 1, 100); Tooltip("Max noise both negative and positive");

}

/// <summary>
/// Uses random noise generation to generate noise heights.
/// </summary>
void TM_InterfaceNOISE::ApplyFunction(float terrainSize, int resolution, float* hMap, NoiseParams params) {
	srand(TM_InterfaceMASTER::seed);
	FlattenHeightMap(hMap, resolution);
	float height = 0.0f;

	//Scale everything so that the look is consistent across terrain resolutions
	const float scale = terrainSize / (float)resolution;
	const double XAmplitude = params.maxNoiseHeight;
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {


			height = rand() % ((int)ceil((XAmplitude)));
			height -= ((int)ceil(XAmplitude / 2));
			hMap[(j * resolution) + i] = height;
		}
	}
}
