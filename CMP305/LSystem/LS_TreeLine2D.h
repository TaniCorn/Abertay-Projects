#pragma once
#include "LSystemSets.h"
class LS_TreeLine2D :
    public LSystemSets
{
public:
    LS_TreeLine2D();
    void Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line) override;
    void Control() override;
    float rotation = 45;
};

