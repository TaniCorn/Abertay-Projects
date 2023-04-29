#include "App1.h"

App1::App1()
{
	//m_Terrain = nullptr;
	tripleTerrain = nullptr;
	shader = nullptr;
	mountainShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load textures
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"snow", L"res/snow.jpeg");
	textureMgr->loadTexture(L"dirt", L"res/dirt.jpg");
	textureMgr->loadTexture(L"sand", L"res/sand.jpeg");
	textureMgr->loadTexture(L"white", L"res/DefaultDiffuse.png");
	landTextures[0] = textureMgr->getTexture(L"grass");
	landTextures[1] = textureMgr->getTexture(L"snow");
	landTextures[2] = textureMgr->getTexture(L"dirt");
	landTextures[3] = textureMgr->getTexture(L"sand");
	// Create Mesh object and shader object
	//m_Terrain = new TerrainMesh(renderer->getDevice(), renderer->getDeviceContext());
	tripleTerrain = new TripleTerrain(renderer->getDevice(), renderer->getDeviceContext());
	shader = new LightShader(renderer->getDevice(), hwnd);
	mountainShader = new MountainShader(renderer->getDevice(), hwnd);
	
	// Initialise light
	light = new Light();

	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(0.5f, -0.6f, 0.5f);


	// Create Mesh object and shader object
	m_Line.reset(new LineMesh(renderer->getDevice(), renderer->getDeviceContext()));

	treeLine2D = new LS_TreeLine2D();
	treeLine3D = new LS_TreeLine3D();
	baseSystem = treeLine3D;
	lSystem.SetAxiom(baseSystem->GetAxiom());
	lSystem.SetRules(baseSystem->GetRules());

	m_leaf = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	//if (m_Terrain)
	//{
	//	delete m_Terrain;
	//	m_Terrain = 0;
	//}
	if (tripleTerrain)
	{
		delete tripleTerrain;
		tripleTerrain = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}	
	
	if (mountainShader)
	{
		delete mountainShader;
		mountainShader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.f, 0.58f, 0.5f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	RenderTerrain();

	
	RenderLSystem();
	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}
/// <summary>
/// Moves the terrain if some are disabled
/// </summary>

void App1::DynamicGrid(int& x, int& z, int& iteration)
{
	if (iteration == 1)
	{
		z = 100;
		x = 0;
	}
	if (iteration == 2)
	{
		x = 100;
		z = 0;
	}
}
void App1::RenderTerrain()
{

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	//Used for the grid system
	int x = 0;
	int z = 0;
	int iteration = 0;
	//Master
	if (tripleTerrain->tmVisual[1])
	{
		tripleTerrain->tm[1]->sendData(renderer->getDeviceContext());
		mountainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, landTextures, light,
			XMFLOAT4(tripleTerrain->snowThreshold, tripleTerrain->dirtThreshold[0], tripleTerrain->dirtThreshold[1], tripleTerrain->sandThreshold),
			XMFLOAT4(tripleTerrain->sandBF, tripleTerrain->grassBF, tripleTerrain->dirtBF, tripleTerrain->snowBF));
		mountainShader->render(renderer->getDeviceContext(), tripleTerrain->tm[1]->getIndexCount());
		iteration++;
		if (tripleTerrain->water[1]->enabled)
		{
			tripleTerrain->water[1]->sendData(renderer->getDeviceContext());
			shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), light, debug);
			shader->render(renderer->getDeviceContext(), tripleTerrain->water[1]->getIndexCount());
			tripleTerrain->water[1]->timecount += timer->getTime();
			tripleTerrain->water[1]->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}

	}
	DynamicGrid(x, z, iteration);
	//Preview
	if (tripleTerrain->tmVisual[0])
	{
		tripleTerrain->tm[0]->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, landTextures[0], light, debug);
		shader->render(renderer->getDeviceContext(), tripleTerrain->tm[0]->getIndexCount());
		iteration++;

		if (tripleTerrain->water[1]->enabled)
		{
			tripleTerrain->water[0]->sendData(renderer->getDeviceContext());
			shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), light, debug);
			shader->render(renderer->getDeviceContext(), tripleTerrain->water[0]->getIndexCount());
		}
	}
	DynamicGrid(x, z, iteration);

	//Composite
	if (tripleTerrain->tmVisual[2])
	{
		tripleTerrain->tm[2]->sendData(renderer->getDeviceContext());
		mountainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, landTextures, light,
			XMFLOAT4(tripleTerrain->snowThreshold, tripleTerrain->dirtThreshold[0], tripleTerrain->dirtThreshold[1], tripleTerrain->sandThreshold),
			XMFLOAT4(tripleTerrain->sandBF, tripleTerrain->grassBF, tripleTerrain->dirtBF, tripleTerrain->snowBF));
		mountainShader->render(renderer->getDeviceContext(), tripleTerrain->tm[2]->getIndexCount());
		iteration++;
		if (tripleTerrain->water[1]->enabled)
		{
			tripleTerrain->water[2]->sendData(renderer->getDeviceContext());
			shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(x, 0, z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), light, debug);
			shader->render(renderer->getDeviceContext(), tripleTerrain->water[2]->getIndexCount());
		}

	}


	DynamicGrid(x, z, iteration);

}
static int lSystemIterations = 0;
static int lSystemType = 0;
void App1::RenderLSystem()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	if (m_Line->getIndexCount() > 0) {
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, debug);
		m_Line->sendData(renderer->getDeviceContext());
		shader->render(renderer->getDeviceContext(), m_Line->getIndexCount());
	}
	if ((treeLine3D->saves.size() > 0))
	{
		if (treeLine3D->saves[0].size() > 0)
		{

			srand(0);
			int treeSize = (treeLine3D->saves[0].size() - 1);
			int treeHeight = (log(treeSize) / log(3)) + 1;
			srand(treeSeed);

			bool uniqueTrees = treeLine3D->uniqueTrees;
			for (int uniqueTreeID = 0; uniqueTreeID < treeLine3D->uniqueTreeAmount; uniqueTreeID++)
			{
				XMFLOAT3 terrainPoint = tripleTerrain->GetRandomPointOnTerrain(100, aboveSnow);
				XMMATRIX terrainMove = XMMatrixIdentity() * XMMatrixTranslation(terrainPoint.x, terrainPoint.y, terrainPoint.z);
				int leafNumber = 0;
				for (int i = 0; i < treeLine3D->saves[0].size(); i++)
				{

					XMVECTOR pos = treeLine3D->saves[uniqueTreeID * uniqueTrees][i].position;
					XMVECTOR dir = XMVectorSet(0, 1, 0, 1);	//Current direction is "Up"
					XMVECTOR right = XMVectorSet(1, 0, 0, 1);	//Rotation axis. Our rotations happen around the "forward" vector
					XMMATRIX currentRotation = treeLine3D->saves[uniqueTreeID * uniqueTrees][i].rotation;

					worldMatrix = renderer->getWorldMatrix();
					worldMatrix *= XMMatrixScaling(1, 10, 1);
					worldMatrix *= currentRotation;
					worldMatrix *= XMMatrixTranslationFromVector(pos);
					worldMatrix *= terrainMove;
					//worldMatrix *= XMMatrixTranslation(m_Line->GetLineList()[i].start.;
					m_Cylinder[i]->sendData(renderer->getDeviceContext());
					shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"dirt"), light, debug);
					shader->render(renderer->getDeviceContext(), m_Cylinder[i]->getIndexCount());

					//We are at a ending
					int currentHeight = treeLine3D->saves[uniqueTreeID * uniqueTrees][i].height;
					if (currentHeight == (treeHeight - 1))
					{

						pos = treeLine3D->savesLeaf[uniqueTreeID * uniqueTrees][leafNumber].position;
						currentRotation = treeLine3D->savesLeaf[uniqueTreeID * uniqueTrees][leafNumber].rotation;
						worldMatrix = renderer->getWorldMatrix();
						float size = 0.3f;
						worldMatrix *= XMMatrixScaling(size, size, size);
						worldMatrix *= currentRotation;
						worldMatrix *= XMMatrixTranslationFromVector(pos);
						worldMatrix *= terrainMove;


						m_leaf->sendData(renderer->getDeviceContext());
						shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, debug);
						shader->render(renderer->getDeviceContext(), m_leaf->getIndexCount());

						leafNumber++;

					}
				}

			}

		}
	}
}
void App1::LSystemGUI()
{
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "LSystem Control");
	ImGui::Checkbox("Debug", &debug);
	ImGui::SliderInt("L System  Type", &lSystemType, 0, 1);
	std::string text;
	switch (lSystemType)
	{
	case 0:
		baseSystem = treeLine3D;
		text = "3D Tree Line System";
		break;
	case 1:
		baseSystem = treeLine2D;
		text = "2D Tree Line System";
		break;
	default:
		baseSystem = treeLine3D;
		text = "3D Tree Line System";
		break;
	}
	lSystem.SetAxiom(baseSystem->GetAxiom());
	lSystem.SetRules(baseSystem->GetRules());

	ImGui::Spacing();
	ImGui::Spacing();
	//L system iteraions and build
	int prevValue = lSystemIterations;
	ImGui::SliderInt("System Iterations", &lSystemIterations, 0, 7);
	if (prevValue != lSystemIterations) {
		lSystem.Run(lSystemIterations);
		BuildLSystem();
	}
	ImGui::SliderInt("Seed", &treeSeed, 0, 100);
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Single System Control");
	baseSystem->Control();
	ImGui::Checkbox("AboveSnow", &aboveSnow);

	

	//Show L system details
	ImGui::Text(text.c_str());
	ImGui::LabelText("L-System", "");
	ImGui::LabelText(lSystem.GetAxiom().c_str(), "Axiom:");
	ImGui::Text("Rules:");
	for each (auto rule in lSystem.GetRules())
	{
		std::string ruleString;
		ruleString.push_back(rule.first);
		ruleString += ": ";
		ruleString.append(rule.second);

		ImGui::Text(ruleString.c_str());
	}

	ImGui::Text("System:");
	ImGui::TextWrapped(lSystem.GetCurrentSystem().c_str());

	

	if (baseSystem->update)
	{
		BuildLSystem();
	}

}

void App1::BuildLSystem()
{
	
	m_Line->Clear();
	baseSystem->Build(&lSystem, m_Line);
	/*for (int i = 0; i < m_leaf.size(); i++)
	{
		delete m_leaf[i];
	}
	m_leaf.clear();
	for (int i = 0; i < treeLine3D->savesLeaf.size(); i++)
	{
		m_leaf.push_back(new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1));

	}*/

	for (int i = 0; i < m_Cylinder.size(); i++)
	{
		delete m_Cylinder[i];
	}
	m_Cylinder.clear();
	if (treeLine3D->saves.size() > 0)
	{

		float startingSize = 0.2;
		int treeSize = (treeLine3D->saves[0].size() - 1);
		int treeHeight = (log(treeSize) / log(3)) + 1;

		float sizeDecrease = startingSize / treeHeight;
		float bottomSize = startingSize;
		float topSize = startingSize - sizeDecrease;

		m_Cylinder.push_back(new CylinderMesh(renderer->getDevice(), renderer->getDeviceContext(), 1, 6, 0.1, bottomSize, topSize));

		for (int i = 1; i < treeLine3D->saves[0].size(); i++)
		{
			int currentHeight = treeLine3D->saves[0][i].height;
			bottomSize = startingSize - (currentHeight * sizeDecrease);
			topSize = startingSize - ((currentHeight + 1) * sizeDecrease);
			m_Cylinder.push_back(new CylinderMesh(renderer->getDevice(), renderer->getDeviceContext(), 1, 6, 0.1, bottomSize, topSize));

		}
	}

	m_Line->BuildLine(renderer->getDeviceContext(), renderer->getDevice());

}


static int fpsCheck = 5;
static bool fpsCatch = false;
void App1::gui()
{
	bool valueChange = false;

	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);
	if (fpsCatch)
	{
		if (timer->getFPS() < 4)
			fpsCheck--;
		else
			fpsCheck = 5;

		if (fpsCheck <= 0)
		{
			tripleTerrain->SafetyReset();
			//m_Terrain->SafetyReset();
		}
	}
	

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat("Camera Speed", &camera->speedCam, 1, 50);
	//ImGui::BeginTooltip();
	ImGui::Checkbox("Safety FPS Catch", &fpsCatch);
	if (ImGui::IsItemHovered())
	{

		ImGui::SetTooltip("If your fps drops below 3fps for an extended time, the map will reset");
	}


		//if (m_Terrain->resizeTerrain) {
		//	m_Terrain->resizeTerrain = false;
		//	m_Terrain->Resize(m_Terrain->GetResolution());
		//	m_Terrain->currentInterface->update = true;
		//	
		//	
		//}
		if (tripleTerrain->resizeTerrain)
		{
			tripleTerrain->resizeTerrain = false;
			tripleTerrain->Resize(tripleTerrain->GetResolution());
			tripleTerrain->currentInterface->update = true;
		}

		if (tripleTerrain->currentInterface != nullptr)
		{
			if (tripleTerrain->currentInterface->update)
			{
				tripleTerrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				tripleTerrain->currentInterface->update = false;
			}
		}
		//if (m_Terrain->currentInterface != nullptr) {
		//	if (m_Terrain->currentInterface->update)
		//	{
		//		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());

		//	}
		//}
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "NOTE: You may hover over certain controls and there may be a tooltip");
		
		if (ImGui::TreeNode("How to use Land Terrain"))
		{
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "How to Use the Land Terrain");
			ImGui::TextColored(ImVec4(0.7, 0, 0, 1), "Presets");
			ImGui::TextWrapped("Recommended to first check out the 'Presets' section and try out some preset generated terrain there. The presets generate a type of terrain with controlled but random parameters.");
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.7, 0, 0, 1), "Height Map Parameters");
			ImGui::TextWrapped("The 'Height Map Parameters' section contains basic heightmap controls. You may turn off different heightmaps, change the thresholds for snow or change the resolution of the terrain. WARNING: Going above 200 resolution could slow the program down.");
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.7, 0, 0, 1), "Function Parameters");
			ImGui::TextWrapped("The most important section is the 'Function Parameters' section, which contains all the individual techniques and methods of terrain generation.");
			ImGui::TextWrapped("When using the functions, you'll see a realtime change in the Preview Heightmap(Top Left). Your Master Heightmap(Center) is the result and the Composition Heightmap(Top Right) is a preview of what would happen if you added your Master and Preview");
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("How to use LSystems"))
		{
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "How to Use the LSystem");
			ImGui::TextColored(ImVec4(0.7, 0, 0, 1), "LSystem Visuals:");
			ImGui::TextWrapped("There are 2 Lsystem types in place. A 3D Lsystem and a 2D Lystem. Both will spawn the line equivalant of their generation at the bottom corner of the master heightmap");
			ImGui::TextWrapped("The 3D Lsystem will also spawn the 3D trees randomly on the MASTER terrain, above the sand height.");
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.7, 0, 0, 1), "Controls");
			ImGui::TextWrapped("First start off by increasing the system iterations. I would recommend keeping it under 6. Then if you are on L System Type: 0, try varying the tree amount. This will increase the amount of trees on the terrain.");
			ImGui::TextWrapped("Unique trees will enable the 3D trees to be stochastic and will make every tree unique, instead of the same one. You can vary the randomness with stochastic range. The Pitch and Roll angle sets the initial parameters and the stochastic range is tied within those.");
			ImGui::TreePop();
		}

		if (ImGui::CollapsingHeader("Land Terrain")) {
			//m_Terrain->HandleGUI();
			tripleTerrain->HandleGUI();

		}
		if (ImGui::CollapsingHeader("Water Terrain"))
		{
			ImGui::Text("It is not recommended changing anything here. The water is very... not happy");
			tripleTerrain->water[1]->HandleGUI();

		}		
		if (ImGui::CollapsingHeader("LSystem"))
		{
			LSystemGUI();
		}

		//if (m_Water->resizeTerrain) {
		//	m_Water->resizeTerrain = false;
		//	m_Water->Resize(m_Water->GetResolution());
		//	m_Water->currentInterface->update = true;
		//	
		//	
		//}
		//if (m_Water->currentInterface != nullptr) {
		//	if (m_Water->currentInterface->update)
		//	{
		//		m_Water->Regenerate(renderer->getDevice(), renderer->getDeviceContext());

		//	}
		//}

		//if (ImGui::CollapsingHeader("Water Terrain"))
		//{
		//	ImGui::Checkbox("Water Enabled", &water);
		//	if (water)
		//		m_Water->HandleGUI();
		//}
		//	


	tripleTerrain->historyInterface.HandleGUI();
	if (tripleTerrain->historyInterface.opened)
	{
		tripleTerrain->historyInterface.opened = false;
		tripleTerrain->UseHistory();
	}
	//tripleTerrain->InImage(renderer->getDevice());
	//ImGui::ShowDemoWindow();
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


