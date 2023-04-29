#include "LS_TreeLine2DThick.h"
#define AI_DEG_TO_RAD(x) ((x)*(ai_real)0.0174532925)
LS_TreeLine2DThick::LS_TreeLine2DThick()
{
	axiom = "F";
	rules['F'] = "F[+F]F[-F]F";
}
void LS_TreeLine2DThick::Build(LSystem* lsystem, std::unique_ptr<LineMesh>& m_Line)
{
	//Get the current L-System string, right now we have a place holder
	std::string systemString = lsystem->GetCurrentSystem();

	//Initialise some variables
	XMVECTOR pos = XMVectorReplicate(0.0f);	//Current position (0,0,0)
	XMVECTOR dir = XMVectorSet(0, 0.1, 0, 1);	//Current direction is "Up"
	XMVECTOR fwd = XMVectorSet(0, 0, 1, 1);	//Rotation axis. Our rotations happen around the "forward" vector
	XMMATRIX currentRotation = XMMatrixRotationRollPitchYaw(0, 0, 0);
	std::stack<SaveStack> saveStack;
	SaveStack save;
	//'A', "[&FA][>&FA][<&FA]"
	// 	//F[&FA][>&FA][<&FA]
	//Go through the L-System string
	for (int i = 0; i < systemString.length(); i++) {
		switch (systemString[i]) {
		case 'F'://Place a branch and move along the branch
			m_Line->AddLine(pos, pos + XMVector3Transform(dir, currentRotation));	//Add the line segment to the line mesh
			pos += XMVector3Transform(dir, currentRotation);						//Move the position marker
			break;
		case '+'://Rotate left
			currentRotation *= XMMatrixRotationAxis(fwd, AI_DEG_TO_RAD(rotation));
			break;
		case '-'://Rotate right
			currentRotation *= XMMatrixRotationAxis(fwd, -AI_DEG_TO_RAD(rotation));
			break;
		case '[':
			save.position = pos;
			save.rotation = currentRotation;
			saveStack.push(save);
			break;
		case ']':
			save = saveStack.top();
			saveStack.pop();
			pos = save.position;
			currentRotation = save.rotation;
			break;
		default:
			break;
		}
	}
}

void LS_TreeLine2DThick::Control()
{
	update = false;
	float prevValue = rotation;
	ImGui::SliderFloat("Rotation Angle", &rotation, 0, 360);
	if (rotation != prevValue)
		update = true;
}
