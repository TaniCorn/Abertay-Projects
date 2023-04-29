#pragma once
#ifndef TM_INTERFACEBASIC
#define TM_INTERFACEBASIC

#include "TM_InterfaceMASTER.h"
class TM_InterfaceBASIC :
    public TM_InterfaceMASTER
{
public:
	struct WaveParams {
		float frequency[2];
		float displacement[2];
		float amplitude[2];
	};
	WaveParams basicParams;

	TM_InterfaceBASIC() {fileName = "TM_BASIC_PARAMETERS.txt";}
	void HandleGUI() override;

	void DefaultParameters() {
		basicParams.frequency[0] = 0.2f;
		basicParams.frequency[1] = 0.2f;
		basicParams.displacement[0] = 0;
		basicParams.displacement[1] = 0;
		basicParams.amplitude[0] = 5.0f;
		basicParams.amplitude[1] = 5.0f;
		autoScrollX = false;
		autoScrollZ = false;
	};

	HistoryKey MakeHistoryKey(int smoothingPost, int smoothingAmount, bool ridgify, int ridgeType, bool bounds, int lowerBound, int upperBound) override{
		HistoryKey h;
		h.smoothingPost = smoothingPost;
		h.smoothAmount = smoothingAmount;
		h.ridgify = ridgify;
		h.ridgeType = ridgeType;
		h.bounds = bounds;
		h.lowerBound = lowerBound;
		h.upperBound = upperBound;
		h.function = "BASIC";
		h.parameters.push_back(std::to_string(basicParams.frequency[0]));
		h.parameters.push_back(std::to_string(basicParams.frequency[1]));
		h.parameters.push_back(std::to_string(basicParams.displacement[0]));
		h.parameters.push_back(std::to_string(basicParams.displacement[1]));
		h.parameters.push_back(std::to_string(basicParams.amplitude[0]));
		h.parameters.push_back(std::to_string(basicParams.amplitude[1]));
		
		return h;
	}
	void UseHistoryKey(HistoryKey h) {
		basicParams.frequency[0] = std::stof(h.parameters[0]);
		basicParams.frequency[1] = std::stof(h.parameters[1]);
		basicParams.displacement[0] = std::stof(h.parameters[2]);
		basicParams.displacement[1] = std::stof(h.parameters[3]);
		basicParams.amplitude[0] = std::stof(h.parameters[4]);
		basicParams.amplitude[1] = std::stof(h.parameters[5]);
	}

	/// <summary>
	/// Applies a sin and cosine wave
	/// </summary>
	static void ApplyFunction(float terrainSize, int resolution, float* hMap, WaveParams waveParams);
	//static void GerstnerWaveFunction(float terrainSize, int resolution, float* hMap, WaveParams waveParams);


private:
	bool autoScrollX = false;
	bool autoScrollZ = false;
};

#endif // !TM_INTERFACEBASIC
