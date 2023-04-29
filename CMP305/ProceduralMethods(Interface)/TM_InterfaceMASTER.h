

#pragma once
#ifndef TM_INTERFACEMASTER
#define TN_INTERFACEMASTER
#include <fstream>
#include <string>
#include <queue>
#include "imGUI/imgui.h"
#include "D3D.h"


/// <summary>
/// Used by the save and load system to save a set of instructions pertaining terrain. See 'TM_InterfaceHISTORY.h'
/// </summary>
struct HistoryKey {
	int smoothingPost;
	int smoothAmount;
	bool ridgify;
	int ridgeType;
	bool bounds;
	int lowerBound, upperBound;
	std::string function;
	std::vector<std::string> parameters;
};

/// <summary>
/// The parent class of all terrain functions. Contains virtual function, smoothing and flat function.
/// </summary>
class TM_InterfaceMASTER
{
public:
	static int seed;//Seed parameter for all functions
	static bool update;//Allows us to continually regenerate the terrain mesh. When set to true, will regenerate
	char* fileName;//All functions must have a fileName to save to

	/// <summary>
	/// All Functions have a GUI menu
	/// </summary>
	virtual void HandleGUI() = 0;
	/// <summary>
	/// All Functions have default 'safe' parameters
	/// </summary>
	virtual void DefaultParameters() = 0;
	/// <summary>
	/// All functions must be able to make a history key for the save and load system. Will save parameters to HistoryKey Parameters
	/// </summary>
	virtual HistoryKey MakeHistoryKey(int smoothingPost, int smoothingAmount, bool ridgify, int ridgeType, bool bounds, int lowerBound, int upperBound) = 0;
	/// <summary>
	/// All Functions must be able to use their history key. Will set parameters to values.
	/// </summary>
	virtual void UseHistoryKey(HistoryKey h) = 0;

	/// <summary>
	/// Legacy: Only contains the smoothing amount
	/// </summary>
	void SmoothGUI();
	int smoothingAmount = 1;

	/// <summary>
	/// Flattens a hieghtmap to 0
	/// </summary>
	static void FlattenHeightMap(float* hMap, int resolution);
	/// <summary>
	/// Smooths a heightmap
	/// </summary>
	static void SmoothHeightMap(float* hMap, int resolution, int smoothingAmount);
	/// <summary>
	/// Foreach height, makes it the absolute value; inverts negative heights to be positive.
	/// </summary>
	static void AbsoluteHeightMap(float* hMap, int resolution);
	/// <summary>
	/// If a height is below 0, make it 0
	/// </summary>
	static void NoZeroHeightMap(float* hMap, int resolution);
	/// <summary>
	/// Raises the entire heightmap by the lowest value, if value is negative, makes it positive
	/// </summary>
	static void RaiseToZeroHeightMap(float* hMap, int resolution);
	static void RidgeHeightMap(float* hMap, int resolution);
	/// <summary>
	/// Tooltip IMGUI function
	/// </summary>
	static void Tooltip(const char* msg) {
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(msg);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//Own implementation of ImGui sliders to update automatically
	static void ValueChange(const char* string, float& parameter, float minParameter, float maxParameter, bool disableWN = false, const char* format = "%.3f", float power = 1.0f);
	static void ValueChange(const char* string, int& parameter, int minParameter, int maxParameter, bool disableWN = false, const char* format = "%d", float power = 1.0f);
	static void ValueChange2(const char* string, int* parameter, int minParameter, int maxParameter, bool disableWN = false, const char* format = "%d", float power = 1.0f);
	static void ValueChange2(const char* string, float* parameter, float minParameter, float maxParameter, bool disableWN = false, const char* format = "%.3f", float power = 1.0f);
	static void RadioChange(const char* string, int* parameter, int num);
};

#endif // !TM_INTERFACEMASTER
