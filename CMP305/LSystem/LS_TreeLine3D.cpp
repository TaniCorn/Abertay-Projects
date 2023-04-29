#include "LS_TreeLine3D.h"
#define AI_DEG_TO_RAD(x) ((x)*(ai_real)0.0174532925)
LS_TreeLine3D::LS_TreeLine3D()
{
	axiom = "FA";
	rules['A'] = "[&FA][>&FA][<&FA]";
}


void LS_TreeLine3D::Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line)
{
	//Get the current L-System string, right now we have a place holder
	std::string systemString = lsystem->GetCurrentSystem();
	saves.clear();
	savesLeaf.clear();

	//If we have unique trees on, run this and generate an array of unique trees
	//Else we run once
	for (int treeID = 0; treeID < ((uniqueTreeAmount-1) * uniqueTrees)+1; treeID++)
	{
		//Initialise some variables
		std::vector<Tree3D> newUniqueTree;//The end result
		std::vector<TreeLeaf3D> newUniqueLeaf;//The end result leafs
		XMVECTOR pos = XMVectorReplicate(0.0f);	//Current position (0,0,0)
		XMVECTOR dir = XMVectorSet(0, 1, 0, 1);	//Current direction is "Up"
		XMVECTOR right = XMVectorSet(1, 0, 0, 1);	//Rotation axis. Our rotations happen around the "forward" vector
		XMMATRIX currentRotation = XMMatrixRotationRollPitchYaw(0, 0, 0);

		std::stack<SaveStack> saveStack;//For Save and Load system
		SaveStack save;//Reusable save
		int height = 0;//Current Height of the tree, get's added every save, and subtracted every load
		int randomStochastic;
		//Go through the L-System string

		//Decides whether to run the stochastic system or not. Code is the same for the most part, just rotating has a random variance
		if (uniqueTrees)
		{
			for (int i = 0; i < systemString.length(); i++) {
				switch (systemString[i]) {
				case 'A'://Control(LEAF)
					//Save a new leaf
					TreeLeaf3D leaf;
					leaf.rotation = currentRotation;
					leaf.position = (pos);
					newUniqueLeaf.push_back(leaf);
					break;
				case 'F'://Place a branch and move along the branch
					//Save a Tree iteration
					Tree3D curTree;
					curTree.position = pos;
					curTree.rotation = currentRotation;
					curTree.height = height;
					newUniqueTree.push_back(curTree);


					m_Line->AddLine(pos, pos + XMVector3Transform(dir, currentRotation));	//Add the line segment to the line mesh
					pos += XMVector3Transform(dir, currentRotation);//Move the position marker
					break;
				case '>'://Roll right
					randomStochastic = rand() % stochasticRange;
					randomStochastic -= (stochasticRange / 2);
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(dir, currentRotation), AI_DEG_TO_RAD(roll + randomStochastic));
					break;
				case '<'://Roll left
					randomStochastic = rand() % stochasticRange;
					randomStochastic -= (stochasticRange / 2);
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(dir, currentRotation), -AI_DEG_TO_RAD(roll + randomStochastic));
					break;
				case '&'://Pitch
					randomStochastic = rand() % stochasticRange;
					randomStochastic -= (stochasticRange / 2);
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(right, currentRotation), AI_DEG_TO_RAD(pitch + randomStochastic));
					break;
				case '['://Save
					height++;
					save.position = pos;
					save.rotation = currentRotation;
					saveStack.push(save);
					break;
				case ']'://Reload
					height--;
					save = saveStack.top();
					saveStack.pop();
					pos = save.position;
					currentRotation = save.rotation;
					break;
				}
			}
		}
		else {
			for (int i = 0; i < systemString.length(); i++) {
				switch (systemString[i]) {
				case 'A'://Control(LEAF)
					//Save a new leaf
					TreeLeaf3D leaf;
					leaf.rotation = currentRotation;
					leaf.position = (pos);
					newUniqueLeaf.push_back(leaf);
					break;
				case 'F'://Place a branch and move along the branch
					//Save a Tree iteration
					Tree3D curTree;
					curTree.position = pos;
					curTree.rotation = currentRotation;
					curTree.height = height;
					newUniqueTree.push_back(curTree);


					m_Line->AddLine(pos, pos + XMVector3Transform(dir, currentRotation));	//Add the line segment to the line mesh
					pos += XMVector3Transform(dir, currentRotation);//Move the position marker
					break;
				case '>'://Roll right
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(dir, currentRotation), AI_DEG_TO_RAD(roll));
					break;
				case '<'://Roll left
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(dir, currentRotation), -AI_DEG_TO_RAD(roll));
					break;
				case '&'://Pitch
					currentRotation *= XMMatrixRotationAxis(XMVector3Transform(right, currentRotation), AI_DEG_TO_RAD(pitch));
					break;
				case '['://Save
					height++;
					save.position = pos;
					save.rotation = currentRotation;
					saveStack.push(save);
					break;
				case ']'://Reload
					height--;
					save = saveStack.top();
					saveStack.pop();
					pos = save.position;
					currentRotation = save.rotation;
					break;
				}
			}

		}

		//Adds the unique tree to saves
		saves.push_back(newUniqueTree);
		savesLeaf.push_back(newUniqueLeaf);
	}

}

void LS_TreeLine3D::Control()
{

	update = false;

	float prevValue = uniqueTrees;
	ImGui::Checkbox("Unique Trees", &uniqueTrees);
	if (prevValue != uniqueTrees)
		update = true;

	 prevValue = uniqueTreeAmount;
	ImGui::SliderInt("Tree Amount", &uniqueTreeAmount, 0, 50);
	if (prevValue != uniqueTreeAmount)
		update = true;


	prevValue = stochasticRange;
	ImGui::SliderInt("Stochastic Range", &stochasticRange, 1, 180);
	if (prevValue != stochasticRange) 		
		update = true;

	 prevValue = roll;
	ImGui::SliderFloat("RollAngle", &roll, 0, 360);
	if (roll != prevValue)
		update = true;


	prevValue = pitch;
	ImGui::SliderFloat("Pitch", &pitch, 0, 360);
	if (pitch != prevValue)
		update = true;
}
