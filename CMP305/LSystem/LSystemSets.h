#pragma once
#include "D3D.h"
#include <string>
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "LineMesh.h"
#include "LSystem.h"
#include <memory>
#include "imGUI/imgui.h"

#include <stack>
#include "LineMesh.h"
#include "LSystem.h"

using std::unique_ptr;

/// <summary>
/// LSystemSets are unique Lsystems with different Axioms and Rules that the LSystem class can run and generate a sequence from
/// </summary>
class LSystemSets
{
public:
	virtual void Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line) = 0;
	virtual void Control() = 0;
	const char* GetAxiom() { return axiom; }
	std::map<char, string> GetRules() { return rules; }
	bool update = false;
protected:
	const char* axiom;
	std::map<char, string> rules;

};

