#pragma once
#ifndef TM_INTERFACEHISTORY
#define TM_INTERFACEHISTORY

#include "TM_InterfaceMASTER.h"
#define PRESET_AMOUNT 5
class TM_InterfaceHISTORY
{
public:
	TM_InterfaceHISTORY() {  };

	/// <summary>
	/// Adds to the history of functions used
	/// </summary>
	void AddHistoryKey(HistoryKey key) {
		currentHistory.push_back(key);
	}
	/// <summary>
	/// Clears history
	/// </summary>
	void ResetHistory() {
		/*int size = currentHistory.size();
		for (int i = 0; i < size; i++)
		{
			currentHistory.pop_back();
		}*/
		currentHistory.clear();
	}
	/// <summary>
/// Changes our current history from being a std::vector to being a std::queue
/// </summary>
	std::queue<HistoryKey> GetQueueHistory() {
		std::queue<HistoryKey> h;
		for (int i = 0; i < currentHistory.size(); i++)
		{
			h.push(currentHistory[i]);
		}
		return h;
	}

	/// <summary>
	/// HistoryInterfaces new UI Menu
	/// </summary>
	void HandleGUI() {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		// Demonstrate the various window flags. Typically you would just use the default!
		static bool no_titlebar = false;
		static bool no_scrollbar = false;
		static bool no_menu = false;
		static bool no_move = false;
		static bool no_resize = false;
		static bool no_collapse = false;
		static bool no_close = false;
		static bool no_nav = false;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
		if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
		if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
		if (no_nav)       window_flags |= ImGuiWindowFlags_NoNav;

		// Main body of the Demo window starts here.
		if (!ImGui::Begin("History of Changes", false, window_flags))
		{
			// Early out if the window is collapsed, as an optimization.
			//opened = false;
			ImGui::End();
			return;
		}
		//opened = true;
		if (ImGui::BeginMenuBar())
		{
			//Top Menu Bar
				if (ImGui::BeginMenu("Save To Preset...")) {
					for (int i = 1; i < PRESET_AMOUNT; i++)
					{
						std::string m = "Preset " + std::to_string(i);
						if (ImGui::MenuItem(m.c_str()))
							SavePreset(i);
					}

					ImGui::EndMenu();
				};
				if (ImGui::BeginMenu("Open...")) {
					for (int i = 1; i < PRESET_AMOUNT; i++)
					{
						std::string m = "Preset " + std::to_string(i);
						if (ImGui::MenuItem(m.c_str()))
							OpenPreset(i);
					}

					ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "After loading a history, it may be required to press 'Apply Function to Master' to view it.");
		//Displaying History
		for (int i = 0; i < currentHistory.size(); i++)
		{
			ImGui::TextColored(ImVec4(0,1,1,1), "HistoryKey:");
			if (currentHistory[i].smoothAmount > 0)
			{
				std::string smo = "Smoothing Type:" + std::to_string(currentHistory[i].smoothingPost);
				ImGui::Text(smo.c_str());
			}

			if (currentHistory[i].bounds)
			{
				std::string bounds = "Lower Bounds:" + std::to_string(currentHistory[i].lowerBound) + ". Upper Bounds:" + std::to_string(currentHistory[i].upperBound);
				ImGui::Text(bounds.c_str());

			}
			if (currentHistory[i].ridgify)
			{
				std::string ridge = "RidgeType:" + std::to_string(currentHistory[i].ridgeType);
				ImGui::Text(ridge.c_str());
			}
			ImGui::Text(currentHistory[i].function.c_str());
			std::string functionNumberString = "Function Parameters " + std::to_string(i);
			if (ImGui::TreeNode(functionNumberString.c_str()))
			{
				for (int j = 0; j < currentHistory[i].parameters.size(); j++)
				{
					ImGui::Text(currentHistory[i].parameters[j].c_str());
				}
				ImGui::TreePop();
			}
		}

		ImGui::End();
	}


///////////////SAVE AND LOAD

	/// <summary>
	/// Takes currentHistory and saves it to a file
	/// </summary>
	/// <param name="num"></param>
	void SavePreset(int num) {
		std::ofstream saveFile;
		std::string m = "Preset "; m.append(std::to_string(num)); m.append(".txt");
		saveFile.open(m);

		//Save all functions
		for (int i = 0; i < currentHistory.size(); i++)
		{
			//Saving 1 function history
			//We save 9 hard values always
			saveFile << currentHistory[i].smoothingPost << std::endl;
			saveFile << currentHistory[i].smoothAmount << std::endl;

			saveFile << int(currentHistory[i].ridgify) << std::endl;
			saveFile << currentHistory[i].ridgeType << std::endl;

			saveFile << int(currentHistory[i].bounds) << std::endl;
			saveFile << currentHistory[i].lowerBound << std::endl;
			saveFile << currentHistory[i].upperBound << std::endl;

			saveFile << currentHistory[i].function << std::endl;
			saveFile << currentHistory[i].parameters.size() << std::endl;
			for (int j = 0; j < currentHistory[i].parameters.size(); j++)
			{
				const char* debug = currentHistory[i].parameters[j].c_str();
				saveFile << debug << std::endl;
			}

		}


		saveFile.close();
	}
	/// <summary>
	/// Resets and loads a history file into 'currentHistory'
	/// </summary>
	void OpenPreset(int num) {
		//Open file
		ResetHistory();
		opened = true;
		std::ifstream saveFile;
		std::string m = "Preset "; m.append(std::to_string(num)); m.append(".txt");
		saveFile.open(m);

		//Read in every single line as string value
		std::queue<std::string> stringVals;
		std::string str;
		while (std::getline(saveFile, str)) {
			stringVals.push(str);
		}

		while (stringVals.size() > 0)
		{
			HistoryKey h;
			h.smoothingPost = std::atoi(stringVals.front().c_str());
			stringVals.pop();
			h.smoothAmount = std::atoi(stringVals.front().c_str());
			stringVals.pop();

			h.ridgify = std::atoi(stringVals.front().c_str());
			stringVals.pop();
			h.ridgeType = std::atoi(stringVals.front().c_str());
			stringVals.pop();

			h.bounds = std::atoi(stringVals.front().c_str());
			stringVals.pop();
			h.lowerBound = std::atoi(stringVals.front().c_str());
			stringVals.pop();
			h.upperBound = std::atoi(stringVals.front().c_str());
			stringVals.pop();
			
			std::string funcName = stringVals.front();
		
		
			//Get function name and parameters
			h.function = funcName;
			stringVals.pop();
			std::string size = stringVals.front();
			int ems = std::atoi(size.c_str());
			for (int i = 0; i < ems; i++)
			{
				stringVals.pop();
				h.parameters.push_back(stringVals.front());

			}
			stringVals.pop();
			AddHistoryKey(h);
			
		}


		saveFile.close();

		//opened = true;
	}

	bool opened = false;

private:
	std::vector<HistoryKey> currentHistory;
};

#endif // !TM_INTERFACEHISTORY
