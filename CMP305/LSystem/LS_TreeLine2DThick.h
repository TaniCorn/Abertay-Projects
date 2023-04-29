#pragma once
#include "LSystemSets.h"
class LS_TreeLine2DThick :
    public LSystemSets
{
public:
    LS_TreeLine2DThick();
    void Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line) override;
    void Control() override;
    

    float rotation = 45;
};

