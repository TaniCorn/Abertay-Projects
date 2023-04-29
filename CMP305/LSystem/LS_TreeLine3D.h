#pragma once
#include "LSystemSets.h"
#include "CylinderMesh.h"

struct Tree3D {
    XMMATRIX rotation;
    XMVECTOR position;
    int height;
};
struct TreeLeaf3D {
    XMVECTOR position;
    XMMATRIX rotation;

};

class LS_TreeLine3D :
    public LSystemSets
{
public:
    LS_TreeLine3D();

    void Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line) override;
    void Build(LSystem* lsystem, std::unique_ptr<CylinderMesh>& cylinder);
    void Control() override;
    //float yaw =120;
    float roll = 120;
    float pitch = 30;

    bool uniqueTrees = false;
    int uniqueTreeAmount = 1;
    int stochasticRange = 60;

    std::vector<std::vector<Tree3D>> saves;
    std::vector<std::vector<TreeLeaf3D>> savesLeaf;
};

