#pragma once
#ifndef TM_INTERFACEPERLIN
#define TM_INTERFACEPERLIN

#include "TM_InterfaceMASTER.h"
#include "PerlinNoise.h"

class TM_InterfacePERLIN : public TM_InterfaceMASTER
{
public:
	struct PerlinParams {
		bool warpFunc = false;
		float xFrequency = 0.002;
		float yFrequency = 0.002;
		float amplitude = 1.0f;

		float xDisplacement = 0.2f;
		float yDisplacement = 0.0f;
		int fractal = 1;
		float frequencyMultiplier = 2;
		float amplitudeMultiplier = 0.5f;

		bool warping = false;
		//bool ridging = false;
		float warpAmp = 1;
		float fbmAmp = 1;
		float warpedXDisplacement = 0;
		float warpedYDisplacement = 0;

		bool autoScrollX = false;
		bool autoScrollZ = false;
	};
	PerlinParams perlinParams;
	bool warp = false;//Changes UI to being the warped function UI
	TM_InterfacePERLIN() { fileName = "TM_PERLIN_PARAMETERS.txt"; };

	void HandleGUI() override;


	void DefaultParameters() {
		perlinParams.warpFunc = false;
		perlinParams.xFrequency = 0.002f;
		perlinParams.yFrequency = 0.002f;
		perlinParams.amplitude = 1.0f;
		perlinParams.xDisplacement = 0.2f;
		perlinParams.yDisplacement = 0.2f;
		perlinParams.fractal = 1;
		perlinParams.frequencyMultiplier = 2.f;
		perlinParams.amplitudeMultiplier = 0.5f;
		perlinParams.warping = false;
		perlinParams.warpAmp = 1;
		perlinParams.fbmAmp = 1;
		perlinParams.warpedXDisplacement = 0;
		perlinParams.warpedYDisplacement = 0;
		perlinParams.autoScrollX = false;
		perlinParams.autoScrollZ = false;
	};
	HistoryKey MakeHistoryKey(int smoothingPost, int smoothingAmount, bool ridgify, int ridgeType, bool bounds, int lowerBound, int upperBound) override {
		HistoryKey h;
		h.smoothingPost = smoothingPost;
		h.smoothAmount = smoothingAmount;
		h.ridgify = ridgify;
		h.ridgeType = ridgeType;
		h.bounds = bounds;
		h.lowerBound = lowerBound;
		h.upperBound = upperBound;
		h.function = "PERLIN";

		h.parameters.push_back(std::to_string(int(perlinParams.warpFunc)));
		h.parameters.push_back(std::to_string(perlinParams.xFrequency));
		h.parameters.push_back(std::to_string(perlinParams.yFrequency));
		h.parameters.push_back(std::to_string(perlinParams.amplitude));
		h.parameters.push_back(std::to_string(perlinParams.xDisplacement));
		h.parameters.push_back(std::to_string(perlinParams.yDisplacement));
		h.parameters.push_back(std::to_string(perlinParams.fractal));
		h.parameters.push_back(std::to_string(perlinParams.frequencyMultiplier));
		h.parameters.push_back(std::to_string(perlinParams.amplitudeMultiplier));
		h.parameters.push_back(std::to_string(int(perlinParams.warping)));
		h.parameters.push_back(std::to_string(perlinParams.warpAmp));
		h.parameters.push_back(std::to_string(perlinParams.fbmAmp));
		h.parameters.push_back(std::to_string(perlinParams.warpedXDisplacement));
		h.parameters.push_back(std::to_string(perlinParams.warpedYDisplacement));
	
		return h;
	}

	void UseHistoryKey(HistoryKey h) {
		int i = 0;
		perlinParams.warpFunc = std::stoi(h.parameters[i++]);
		perlinParams.xFrequency = std::stof(h.parameters[i++]);
		perlinParams.yFrequency = std::stof(h.parameters[i++]);
		perlinParams.amplitude = std::stof(h.parameters[i++]);
		perlinParams.xDisplacement = std::stof(h.parameters[i++]);
		perlinParams.yDisplacement = std::stof(h.parameters[i++]);
		perlinParams.fractal = std::stoi(h.parameters[i++]);
		perlinParams.frequencyMultiplier = std::stof(h.parameters[i++]);
		perlinParams.amplitudeMultiplier = std::stof(h.parameters[i++]);
		perlinParams.warping = std::stoi(h.parameters[i++]);
		perlinParams.warpAmp = std::stof(h.parameters[i++]);
		perlinParams.fbmAmp = std::stof(h.parameters[i++]);
		perlinParams.warpedXDisplacement = std::stof(h.parameters[i++]);
		perlinParams.warpedYDisplacement = std::stof(h.parameters[i++]);
		
	}

	/// <summary>
	/// Applies basic perlin noise
	/// </summary>
	static void ApplyFunction(int resolution, float* hMap, PerlinParams params);
	/// <summary>
	/// Applies fractal perlin noise with optional pseudo warping
	/// </summary>
	static void FractalFunction(int resolution, float* hMap, PerlinParams params);
	/// <summary>
	/// Applies True Warped noise
	/// </summary>
	/// <param name="resolution"></param>
	/// <param name="hMap"></param>
	/// <param name="params"></param>
	static void WarpedFunction(int resolution, float* hMap, PerlinParams params);
	/// <summary>
	/// Gets a singular perlin noise value
	/// </summary>
	static float NoiseSample(int resolution, int x, int y, int xFrequency, int yFrequency, float xDisplacement, float yDisplacement);
public:
private:
	/// <summary>
	/// Gets the fractional brownian motion of a perlin noise height
	/// </summary>
	static float FBMFunction(double xAx, double yAx, PerlinParams params);
	/// <summary>
/// Gets the fractional brownian motion of a perlin noise height
/// </summary>
	static float FBMFunction(XMFLOAT2 point, PerlinParams params);

};

#endif // !TM_INTERFACEPERLIN
