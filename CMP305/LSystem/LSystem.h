#pragma once
#include <string>
#include "D3D.h"
#include <map>
#include <vector>
using std::string;
using std::map;

struct SaveStack {
	XMVECTOR position;
	XMMATRIX rotation;
};

/// <summary>
/// LSystem takes a LSystemSet and generates a system out of it.
/// Must set an axiom ann rules before running it.
/// </summary>
class LSystem
{
public:
	LSystem() {};

	/// <summary>
	/// Get the string that represents the current state of the L-System
	/// Get the system after running it.
	/// </summary>
	inline const string GetCurrentSystem() { return m_CurrentSystem; }


	inline void SetAxiom(string Axiom) { m_Axiom = Axiom; }
	inline const string GetAxiom() { return m_Axiom; }
	inline const map<char, string> GetRules() { return rules; }

	void AddRule(const char, const string);		//Add a rule to the system
	void SetRules(map<char,string> rule);		//Add a rule to the system
	void ClearRules() {
		rules.clear();
	}

	void Run(const int count);					//Run the system a set number of times
	
	void Iterate();								//Apply the rules one time
	void Reset();								//Set the system back to its initial state
private:
	string m_Axiom;
	string m_CurrentSystem;
	map<char, string> rules;
};

