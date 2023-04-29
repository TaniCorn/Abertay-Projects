#pragma once
#ifndef TM_INTERFACEFAULTING
#define TM_INTERFACEFAULTING

#include "TM_InterfaceMASTER.h"
/// <summary>
/// Faulting terrain interface
/// </summary>
class TM_InterfaceFAULTING : public TM_InterfaceMASTER
{
public:
	struct FaultingParams {
		int faultingIterations = 0;
		float heightIncrease = 0.5f;
		float negativeMultiplier = 1.0f;

		bool enableRight = true;
		bool enableTop = true;
		bool enableBottom = true;
		bool enableLeft = true;

		int smoothingFrequency = 0;
		int smoothingStrength = 0;
	};
	FaultingParams faultParams;
	TM_InterfaceFAULTING() { fileName = "TM_FAULTING_PARAMETERS.txt"; };

	void HandleGUI() override;


	void DefaultParameters() {
		
		TM_InterfaceMASTER::seed = 0;
		faultParams.faultingIterations = 0;
		faultParams.heightIncrease = 0.5f;
		faultParams.negativeMultiplier = 1.0f;

		faultParams.enableRight = true;
		faultParams.enableTop = true;
		faultParams.enableBottom = true;
		faultParams.enableLeft = true;

		faultParams.smoothingFrequency = 0;
		faultParams.smoothingStrength = 0;
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
		h.function = "FAULTING";
		h.parameters.push_back(std::to_string(TM_InterfaceMASTER::seed));
		h.parameters.push_back(std::to_string(faultParams.faultingIterations));
		h.parameters.push_back(std::to_string(faultParams.heightIncrease));
		h.parameters.push_back(std::to_string(faultParams.negativeMultiplier));
		h.parameters.push_back(std::to_string(faultParams.enableRight));
		h.parameters.push_back(std::to_string(faultParams.enableTop));
		h.parameters.push_back(std::to_string(faultParams.enableBottom));
		h.parameters.push_back(std::to_string(faultParams.enableLeft));
		h.parameters.push_back(std::to_string(faultParams.smoothingFrequency));
		h.parameters.push_back(std::to_string(faultParams.smoothingStrength));

		return h;
	}
	void UseHistoryKey(HistoryKey h) {
		int i = 0;
		TM_InterfaceMASTER::seed = std::stoi(h.parameters[i++]);
		faultParams.faultingIterations = std::stoi(h.parameters[i++]);
		faultParams.heightIncrease = std::stof(h.parameters[i++]);
		faultParams.negativeMultiplier = std::stof(h.parameters[i++]);
		faultParams.enableRight = std::stoi(h.parameters[i++]);
		faultParams.enableTop = std::stoi(h.parameters[i++]);
		faultParams.enableBottom = std::stoi(h.parameters[i++]);
		faultParams.enableLeft = std::stoi(h.parameters[i++]);
		faultParams.smoothingFrequency = std::stoi(h.parameters[i++]);
		faultParams.smoothingStrength = std::stoi(h.parameters[i++]);
	}
public:


	/// <summary>
	/// Applies the faultinga algorithm. Split the terrain with a line, one half goes up, one half goes down.
	/// </summary>
	static void ApplyFunction(float terrainSize, int resolution, float* hMap, FaultingParams params);
private:

	/// <summary>
	/// Applies a single fault line on a heightmap. 
	/// </summary>
	static void Faulting(int resolution, float* hMap, const std::vector<int> directionsAvailable, FaultingParams params);
	/// <summary>
	/// Given a set of directions and interface variables. Chooses the start and end of the faulting line, for "TerrainMesh::BuildFaultingHeightMap()".
	/// </summary>
	static XMFLOAT3 ChooseEdgePoint(int resolution, std::vector<int> directionsAvailable);


private:
	
};

#endif // !TM_INTERFACEFAULTING
