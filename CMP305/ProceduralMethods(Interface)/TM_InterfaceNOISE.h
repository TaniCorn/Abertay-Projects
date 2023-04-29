#pragma once
#ifndef TM_INTERFACENOISE
#define TM_INTERFACENOISE

#include "TM_InterfaceMASTER.h"

class TM_InterfaceNOISE : public TM_InterfaceMASTER
{
public:
	/// <summary>
	/// Parameters of interface and function
	/// </summary>
	struct NoiseParams {
		int maxNoiseHeight = 1;
	};
	NoiseParams noiseParams;
	TM_InterfaceNOISE() { fileName = "TM_NOISE_PARAMETERS.txt"; };
	
	void DefaultParameters() override {
		noiseParams.maxNoiseHeight = 1;
		TM_InterfaceMASTER::seed = 0;
	} ;
	void HandleGUI() override;

	HistoryKey MakeHistoryKey(int smoothingPost, int smoothingAmount, bool ridgify, int ridgeType, bool bounds, int lowerBound, int upperBound) override {
		HistoryKey h;
		h.smoothingPost = smoothingPost;
		h.smoothAmount = smoothingAmount;
		h.ridgify = ridgify;
		h.ridgeType = ridgeType;
		h.bounds = bounds;
		h.lowerBound = lowerBound;
		h.upperBound = upperBound;
		h.function = "NOISE";
		h.parameters.push_back(std::to_string(TM_InterfaceMASTER::seed));
		h.parameters.push_back(std::to_string(noiseParams.maxNoiseHeight));

		return h;
	}

	void UseHistoryKey(HistoryKey h) {
		int i = 0;
		TM_InterfaceMASTER::seed = std::stoi(h.parameters[i++]);
		noiseParams.maxNoiseHeight = std::stoi(h.parameters[i++]);

	}

	/// <summary>
	/// Applies random noise to the entire heightmap for every value
	/// </summary>
	static void ApplyFunction(float terrainSize, int resolution, float* hMap, NoiseParams params);

};

#endif // !TM_INTERFACENOISE
