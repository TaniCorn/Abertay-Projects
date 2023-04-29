#pragma once
#ifndef TM_INTERFACEPARTICLEDEPOSITION
#define TM_INTERFACEPARTICLEDEPOSITION

#include "TM_InterfaceMASTER.h"
#include <set>



class TM_InterfacePARTICLEDEPOSITION : public TM_InterfaceMASTER
{
private:
	struct EmitPoint2 {
	public:
		EmitPoint2() { x = 0; y = 0; heightParameters[0] = -1; heightParameters[1] = -1; };
		EmitPoint2(float x, float y) { this->x = x; this->y = y; heightParameters[0] = -1; heightParameters[1] = -1; };
		EmitPoint2(float x, float y, float heightsMin, float heightsMax) { this->x = x; this->y = y; heightParameters[0] = heightsMin; heightParameters[1] = heightsMax; };
		float x;
		float y;
		float heightParameters[2];
	};

	struct ReverseComparator {
	public:
		bool operator ()(const EmitPoint2& a, const EmitPoint2& b) const {
			return ((a.x < b.x) || ((!(b.x < a.x)) && (a.y < b.y)));
		}
	};
public:
	struct ParticleDepositionParams {
		float heightParameters[2] = { 0, 5 };
		float emitterPoint[2] = { 0.3f,0.3f };//Midpoint of our emitter zone
		float emitterRadius = 0.2f;
		float emitterDensity = 1;

		//Emitters, Emitters have their own height parameters
		std::set<EmitPoint2, ReverseComparator> emitters;
		int iterations = 0;
		bool settingPoint = false;
		bool recursive = false;
		float recursiveHeightThreshold = 0.7f;
		bool fuzz = false;
		int fuzzRadius = 10;
		//bool randomiseHeightPerEmitterOrRunS;
	};
	ParticleDepositionParams pdParams;
	TM_InterfacePARTICLEDEPOSITION() { fileName = "TM_DEPOSITION_PARAMETERS.txt"; };

	void HandleGUI() override;
	void DefaultParameters() {
		TM_InterfaceMASTER::seed = 0;
		pdParams.heightParameters[0] = 0;
		pdParams.heightParameters[1] = 5;

		pdParams.emitterPoint[0] = 0.3f;
		pdParams.emitterPoint[1] = 0.3f;
		pdParams.emitterRadius = 0.2f;
		pdParams.emitterDensity = 1;
		pdParams.emitters.clear();
		pdParams.iterations = 0;
		pdParams.settingPoint = false;
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
		h.function = "PARTICLEDEPOSITION";
		h.parameters.push_back(std::to_string(TM_InterfaceMASTER::seed));
		h.parameters.push_back(std::to_string(pdParams.iterations));
		h.parameters.push_back(std::to_string(int(pdParams.recursive)));
		h.parameters.push_back(std::to_string(pdParams.recursiveHeightThreshold));
		h.parameters.push_back(std::to_string(int(pdParams.fuzz)));
		h.parameters.push_back(std::to_string(pdParams.fuzzRadius));
		for each (auto var in pdParams.emitters)
		{
			h.parameters.push_back(std::to_string(var.x));
			h.parameters.push_back(std::to_string(var.y));
			h.parameters.push_back(std::to_string(var.heightParameters[0]));
			h.parameters.push_back(std::to_string(var.heightParameters[1]));

		}

		return h;
	}
	void UseHistoryKey(HistoryKey h) {
		int i = 0;
		TM_InterfaceMASTER::seed = std::stoi(h.parameters[i++]);
		pdParams.iterations = std::stoi(h.parameters[i++]);
		pdParams.recursive = std::stoi(h.parameters[i++]);
		pdParams.recursiveHeightThreshold = std::stof(h.parameters[i++]);
		pdParams.fuzz = std::stoi(h.parameters[i++]);
		pdParams.fuzzRadius = std::stoi(h.parameters[i++]);
		while (i < h.parameters.size())
		{
			EmitPoint2 point;
			point.x = std::stof(h.parameters[i++]);
			point.y = std::stof(h.parameters[i++]);

			point.heightParameters[0] = (std::stof(h.parameters[i++]));
			point.heightParameters[1] = (std::stof(h.parameters[i++]));
			pdParams.emitters.insert(point);
		}

	}

	

	void MarshLandsParameters();
	void PresetButtons();
	static void AddEmitterPoints(int resolution, ParticleDepositionParams& params);

	/////////Particle Deposition
	/// <summary>
	/// Applies the "TM_InterfacePARTICLEDEPOSITION" implmentation to a heightmap.
	/// </summary>
	static void ApplyFunction( int resolution, float* hMap, ParticleDepositionParams& params);
private:
	static void UserDefinedPoints(int res, float* hMap, ParticleDepositionParams& params);
	static void DisplayCurrentPoint(int res, float* hMap, float xP1, float zP1, float radius, float visualHeightIncrease);
	/// <summary>
/// Grabs random points inside a circle
/// </summary>
	static std::vector<XMFLOAT2> RandomisePointsInCircle(int res, XMINT2 circleOrigin, float circleRadius, float emitterDensity);
	static XMINT2 CheckNeighboursMooreParticleDeposition(int res, float* hMap, XMINT2 particleLocation, float heightThreshold, bool recursive);
	static XMINT2 CheckNeighboursMooreParticleDeposition_Negative(int res, float* hMap, XMINT2 particleLocation, float heightThreshold, bool recursive);
private:
	static int stackOverflow;

};

//Plan of Action:
/*
* Upper heigh and lower height increase is defined by user
* Random height for each particle
* Height difference threshold for dropping beside particle defined by user
* 
* Allow user to specify emitter location and drop a emitter there.
* Then by using iteration slider, all emitter will emit x amounts of times.
* 
* Allow user to specify emitter location and radius. Emitter will randomly choose points in the radius as extra emitter points.
* Then by using iteration slider, all emitter will emit x amounts of times.
* 
* Using a randomness scale slider, the emitters will no longer all emit on the iteration slider.(1 means no randomness)
* All emitters will now have a y chance of emitting a particle
* 
* Allow make emitters negative(before placing down)
* 
* Allow users to click a random button. The world will generate random positive and negative emitters

*/

#endif // !TM_INTERFACEPARTICLEDEPOSITION
