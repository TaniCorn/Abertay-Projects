#include "LSystem.h"

void LSystem::Run(const int count) {
	Reset();
	for (int i = 0; i < count; i++) {
		Iterate();
	}
}

void LSystem::AddRule(char predecessor, string successor) {
	rules[predecessor] = successor;
}

void LSystem::SetRules(map<char, string> rule)
{
	ClearRules();
	for each (auto aRule in rule)
	{
		rules.insert(std::make_pair(aRule.first, aRule.second));
	}
}

void LSystem::Iterate() {
	string newSystem;
	string savePoint;
	for (int i = 0; i < m_CurrentSystem.size(); i++)
	{
		bool ruleFound = false;
		for each (auto rule in rules)
		{
			
			if (m_CurrentSystem[i] == rule.first)
			{
				ruleFound = true;
				newSystem.append(rule.second);
			}
			
		}
		
		if (!ruleFound)
			newSystem.push_back(m_CurrentSystem[i]);
	}
	m_CurrentSystem = newSystem;
}

void LSystem::Reset() {
	m_CurrentSystem = m_Axiom;
}
