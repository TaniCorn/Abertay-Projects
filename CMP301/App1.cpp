// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	textureMgr->loadTexture(L"wood", L"res/wood.png");
	textureMgr->loadTexture(L"Height", L"res/height.png");
	textureMgr->loadTexture(L"Height2", L"res/height2.png");
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());

	manipulatedMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	frankShader = new FrankLunaShader(renderer->getDevice(), hwnd);
	currentShader = frankShader;


	// Set Up Lights
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		lightsInScene[i] = MultiLightShader::LightData();
		lightSphere[i] = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 20);
		lightRepresent[i] = new Light();
		lightRepresent[i]->setDirection(0.0f, -0.7f, 0.7f);
		lightRepresent[i]->setPosition(0.f, 0.f, -10.f);
	}
	
	lightsInScene[0] = MultiLightShader::LightData(XMFLOAT3(1, 50, 1), XMFLOAT3(0, -1, 0), XMFLOAT4(1, 1, 1, 1));
	lightsInScene[0].lightType = 0;
	lightRepresent[0]->setPosition(1, 50, 1);
	lightRepresent[0]->setDirection(0, -1, 0);
	lightRepresent[0]->generateOrthoMatrix(100.0f, 100.0f, 0.1f, 100.0f);
	SetupLightShow();
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	int shadowmapWidth = 1024 * 7;
	int shadowmapHeight = 1024 * 7;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		shadowMap[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}

	quadTessShader = new QuadTessShader(renderer->getDevice(), hwnd);
	manipulatedMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	kernalOM = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	kernalRT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	sceneRT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	kernalFilter = new KernalFilter(renderer->getDevice(), hwnd);
	directionalShader = new DirectionalBlur(renderer->getDevice(), hwnd);
}

void App1::SetupLightShow()
{
	//Directional 1
	lightsInScene[0].enabled = true;
	lightsInScene[0].lightType = 0;
	lightsInScene[0].position = XMFLOAT3(51.163f, 41.860f, 1.0f);
	lightsInScene[0].direction = XMFLOAT3(1.f, -1.f, 0.545f);
	lightsInScene[0].diffuse = XMFLOAT4(69.0f/255.0f, 69.f/255.f,69.f/255.f, 1.0f);	

	//Directional 2
	lightsInScene[1].enabled = true;
	lightsInScene[1].lightType = 0;
	lightsInScene[1].position = XMFLOAT3(41.860f, 38.233f, 63.636f);
	lightsInScene[1].direction = XMFLOAT3(-0.233f, -1.f, -0.277f);
	lightsInScene[1].diffuse = XMFLOAT4(0.f, 0.f,1.0f, 1.0f);	

	//Directional 3
	lightsInScene[2].enabled = true;
	lightsInScene[2].lightType = 0;
	lightsInScene[2].position = XMFLOAT3(0,11.628f, -2.273f);
	lightsInScene[2].direction = XMFLOAT3(-0.116f, -0.326f, 0.386f);
	lightsInScene[2].diffuse = XMFLOAT4(0, 1, 67.0f/255.f, 1.0f);	
	
	//Point Light 1
	lightsInScene[3].enabled = true;
	lightsInScene[3].lightType = 1;
	lightsInScene[3].position = XMFLOAT3(53.488f, 11.682f, 27.273f);
	lightsInScene[3].diffuse = XMFLOAT4(1,0,0, 1.0f);
	lightsInScene[3].specularColor = XMFLOAT4(1,188.0f/255.f,0, 1.0f);
	lightsInScene[3].attenuationValues = XMFLOAT4(0.0652f, 0.0652f, 0.0007f, 1);
	lightsInScene[3].specularPower = 18.243f;	
	
	//Point Light 2
	lightsInScene[4].enabled = true;
	lightsInScene[4].lightType = 1;
	lightsInScene[4].position = XMFLOAT3(86.847, -2.326, 81.818);
	lightsInScene[4].diffuse = XMFLOAT4(187.0f/255.f,0,1,1);
	lightsInScene[4].specularColor = XMFLOAT4(0,1,67.f/255.f, 1.0f);
	lightsInScene[4].attenuationValues = XMFLOAT4(0.4491f,0.0004f,0.0026f, 1);
	lightsInScene[4].specularPower = 26.351f;	
	
	//Spot Light 1
	lightsInScene[5].enabled = true;
	lightsInScene[5].lightType = 2;
	lightsInScene[5].position = XMFLOAT3(25.581,16.605,17.182);
	lightsInScene[5].direction = XMFLOAT3(-1,-0.884,0.364);
	lightsInScene[5].diffuse = XMFLOAT4(1,0,0,1);
	lightsInScene[5].specularColor = XMFLOAT4(1,143.f/255.f,0, 1.0f);
	lightsInScene[5].attenuationValues = XMFLOAT4(0.4762,0.0212,0.0004, 1);
	lightsInScene[5].specularPower = 200.0f;
	lightsInScene[5].spotAngle = 20.0439f;
	
	//Spot Light 2
	lightsInScene[6].enabled = true;
	lightsInScene[6].lightType = 2;
	lightsInScene[6].position = XMFLOAT3(9.302,2.326,11.364);
	lightsInScene[6].direction = XMFLOAT3(0.907,-1,0.023);
	lightsInScene[6].diffuse = XMFLOAT4(1,1,1,1);
	lightsInScene[6].specularColor = XMFLOAT4(1,1,1, 1.0f);
	lightsInScene[6].attenuationValues = XMFLOAT4(0.5,0.0026,0.0001, 1);
	lightsInScene[6].specularPower = 200.0f;
	lightsInScene[6].spotAngle = 50.f;	
	
	//Spot Light 3
	lightsInScene[7].enabled = true;
	lightsInScene[7].lightType = 2;
	lightsInScene[7].position = XMFLOAT3(18.605,6.977,11.364);
	lightsInScene[7].direction = XMFLOAT3(-0.163,-1,0.636);
	lightsInScene[7].diffuse = XMFLOAT4(0,1,142.f/255.f,1);
	lightsInScene[7].specularColor = XMFLOAT4(0,15.f/255.f,1, 1.0f);
	lightsInScene[7].attenuationValues = XMFLOAT4(0.3762,0.0078,0.0004, 1);
	lightsInScene[7].specularPower = 93.919f;
	lightsInScene[7].spotAngle = 10.642f;
	
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
	
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
	if (!wireframeToggle)
	{
		shadowPass();
		sceneRT->setRenderTarget(renderer->getDeviceContext());
		sceneRT->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);
		firstPass();
		renderer->setBackBufferRenderTarget();
		if (postProcess == 1 || postProcess == 0)
		{
			kernalPass();

		}
		else if (postProcess == 2)
		{
			directionalBlurPass();
		}
		render();
	}
	else {
		shadowPass();
		renderer->beginScene(0.0f, 1.0f, 0.f, 1.0f);
		firstPass();
		gui();
		renderer->endScene();

	}

	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::firstPass()
{
	// Set the render target to be the render to texture and clear it


	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render shape with simple lighting shader set.
	manipulatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST);
	quadTessShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(0, -5, 0), viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"Height"), textureMgr->getTexture(L"wood"),
		XMFLOAT2(minMaxLOD[0], minMaxLOD[1]), XMFLOAT4(qtsParameters[0], qtsParameters[1], qtsParameters[2], qtsParameters[3]), 
		steepness, XMFLOAT3(atten[0], atten[1], atten[2]), lightsInScene, shadowMapSRV, shadowOrthoMatrix, shadowViewMatrix, 
		camera->getPosition(), shadowBias, lightingModel, quadExtra );
	quadTessShader->render(renderer->getDeviceContext(), manipulatedMesh->getIndexCount());

	model->sendData(renderer->getDeviceContext());
	currentShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, XMMatrixTranslation(35, 14, 35)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMapSRV, lightsInScene, shadowOrthoMatrix, shadowViewMatrix, camera->getPosition(), shadowBias, lightingModel);
	currentShader->render(renderer->getDeviceContext(), model->getIndexCount());
	
	cubeMesh->sendData(renderer->getDeviceContext());
	currentShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, XMMatrixTranslation(20, 11, 20)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMapSRV, lightsInScene, shadowOrthoMatrix, shadowViewMatrix, camera->getPosition(), shadowBias, lightingModel);
	currentShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (!lightsInScene[i].enabled)
			continue;

		lightSphere[i]->sendData(renderer->getDeviceContext());
		currentShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, XMMatrixTranslation(lightsInScene[i].position.x, lightsInScene[i].position.y, lightsInScene[i].position.z)), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMapSRV, lightsInScene, shadowOrthoMatrix, shadowViewMatrix, camera->getPosition(), shadowBias, lightingModel);
		currentShader->render(renderer->getDeviceContext(), lightSphere[i]->getIndexCount());
	}
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	return true;
}
bool App1::kernalPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)kernalRT->getTextureWidth();
	float screenSizeY = (float)kernalRT->getTextureHeight();
	kernalRT->setRenderTarget(renderer->getDeviceContext());
	kernalRT->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = kernalRT->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	kernalOM->sendData(renderer->getDeviceContext());
	kernalFilter->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, sceneRT->getShaderResourceView(), XMFLOAT2(screenSizeX, screenSizeY), XMINT2(gDist[0], gDist[1]), horizontalTexel, filterType);
	kernalFilter->render(renderer->getDeviceContext(), kernalOM->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	return true;
}

bool App1::directionalBlurPass()
{
		XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

		float screenSizeX = (float)kernalRT->getTextureWidth();
		float screenSizeY = (float)kernalRT->getTextureHeight();
		kernalRT->setRenderTarget(renderer->getDeviceContext());
		kernalRT->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

		worldMatrix = renderer->getWorldMatrix();
		baseViewMatrix = camera->getOrthoViewMatrix();
		orthoMatrix = kernalRT->getOrthoMatrix();

		// Render for Horizontal Blur
		renderer->setZBuffer(false);
		kernalOM->sendData(renderer->getDeviceContext());
		directionalShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, sceneRT->getShaderResourceView(), XMFLOAT2(screenSizeX, screenSizeY), horizontalTexel, XMFLOAT3(directional[0], directional[1], dirStrength), dirSample,dragAmount);
		directionalShader->render(renderer->getDeviceContext(), kernalOM->getIndexCount());
		renderer->setZBuffer(true);

		// Reset the render target back to the original back buffer and not the render to texture anymore.
		renderer->setBackBufferRenderTarget();
		return true;
}



bool App1::render()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 1.0f, 0.f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	kernalOM->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, kernalRT->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), kernalOM->getIndexCount());
	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
	return true;
}

bool App1::shadowPass()
{

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (!lightsInScene[i].enabled)
			continue;

		if (lightsInScene[i].lightType != 0)
			continue;

		shadowMap[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		// get the world, view, and projection matrices from the camera and d3d objects.
		lightRepresent[i]->generateViewMatrix();
		//lightRepresent[i]->generateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
		lightRepresent[i]->generateOrthoMatrix(700.f, 700.0f, 0.1f, 100.0f);

		//XMMATRIX lightViewMatrix = lightref->getViewMatrix();
		XMMATRIX lightViewMatrix = lightRepresent[i]->getViewMatrix();
		XMMATRIX lightOrthoMatrix = lightRepresent[i]->getOrthoMatrix();
		XMMATRIX worldMatrix = renderer->getWorldMatrix();

		//worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

		// Render floor
		if (terrainShadows)
		{
			manipulatedMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST);
			quadTessShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(0, -5, 0), lightViewMatrix, lightOrthoMatrix,
				textureMgr->getTexture(L"Height"), textureMgr->getTexture(L"wood"),
				XMFLOAT2(minMaxLOD[0], minMaxLOD[1]), XMFLOAT4(qtsParameters[0], qtsParameters[1], qtsParameters[2], qtsParameters[3]),
				steepness, XMFLOAT3(atten[0], atten[1], atten[2]), lightsInScene, shadowMapSRV, shadowOrthoMatrix, shadowViewMatrix,
				camera->getPosition(), shadowBias, lightingModel, quadExtra);
			quadTessShader->render(renderer->getDeviceContext(), manipulatedMesh->getIndexCount());
		}
		else {
			manipulatedMesh->sendData(renderer->getDeviceContext());
			depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(0, -5, 0), lightViewMatrix, lightOrthoMatrix);
			depthShader->render(renderer->getDeviceContext(), manipulatedMesh->getIndexCount());
		}



		model->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(35, 14, 35), lightViewMatrix, lightOrthoMatrix);
		depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

		cubeMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(20, 11, 20), lightViewMatrix, lightOrthoMatrix);
		depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

		// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();

		shadowMapSRV[i] = shadowMap[i]->getDepthMapSRV();
		shadowViewMatrix[i] = XMMatrixTranspose(lightViewMatrix);
		shadowOrthoMatrix[i] = XMMatrixTranspose(lightOrthoMatrix);

	}
	return true;
}
static int selected = -1;

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Terrain Normal View", &normalView);

	ImGui::TextWrapped("Note: To initially enable shadows, please go to the lights section and click on lights 0,1, and 2 to update the values and show shadows");
	ImGui::Spacing();
	ImGui::SliderFloat("Camera Speed", &cameraSpeed,1,50);
	camera->setSpeed(cameraSpeed);
	if (true)
	{
		
		if (ImGui::TreeNode("Light and Shadow Settings"))
		{
			ImGui::SliderFloat("Shadow Bias", &shadowBias, 0, 0.3f, "%.5f", 3.0f);
			ImGui::Checkbox("Terrain Shadows", &terrainShadows);
			ImGui::SliderInt("ShaderType", &lightingModel, 0, 3);
			std::string shaderName;
			switch (lightingModel)
			{
			case 0:
				currentShader = frankShader;
				shaderName = "Frank D. Luna Shader";
				break;
			case 1:
				shaderName = "Square Shader";

				break;
			case 2:
				shaderName = "Band Shader";
				break;
			case 3:
				shaderName = "Not Available";
			default:
				shaderName = "Not Available";
				break;
			}
			ImGui::Text(shaderName.c_str());

			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;
			int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

			ImGui::Text("Ambient:"); ImGui::SameLine(); ImGui::ColorEdit3("MyColor##0", (float*)&MultiLightShader::ambientLight, misc_flags);
			ImGui::Columns(3, "Available Lights", false);  // 3-ways, no border


			ImGui::Separator();

			for (int n = 0; n < MAX_LIGHTS; n++)
			{
				std::string title = "Light number " + to_string(n);
				if (ImGui::Selectable(title.c_str(), selected == n)) {
					selected = n;

				}
				ImGui::NextColumn();

			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::TreePop();
			if (selected >= 0)
			{
				std::string title = "Light number " + to_string(selected);

				ImGui::Text(title.c_str());
				float diffuse[4] = { lightsInScene[selected].diffuse.x, lightsInScene[selected].diffuse.y,lightsInScene[selected].diffuse.z,lightsInScene[selected].diffuse.w };
				float specularColor[4] = { lightsInScene[selected].specularColor.x, lightsInScene[selected].specularColor.y,lightsInScene[selected].specularColor.z,lightsInScene[selected].specularColor.w };
				float position[3] = { lightsInScene[selected].position.x, lightsInScene[selected].position.y,lightsInScene[selected].position.z };
				float direction[3] = { lightsInScene[selected].direction.x, lightsInScene[selected].direction.y,lightsInScene[selected].direction.z };
				float atten[4] = { lightsInScene[selected].attenuationValues.x, lightsInScene[selected].attenuationValues.y,lightsInScene[selected].attenuationValues.z,lightsInScene[selected].attenuationValues.w };
				float specPow = lightsInScene[selected].specularPower;
				float spotPow = lightsInScene[selected].spotPower;
				float spotAngle = lightsInScene[selected].spotAngle;
				bool enabled = lightsInScene[selected].enabled;
				int lightType = lightsInScene[selected].lightType;

				ImGui::SliderInt("Light Type", &lightType, 0, 2);
				std::string typeName;
				switch (lightType)
				{
				case 0:
					typeName = "Directional";
					break;
				case 1:
					typeName = "Point";
					break;
				case 2:
					typeName = "Spot";
					break;
				default:
					typeName = "NULL";
					break;
				}
				ImGui::Text(typeName.c_str());

				ImGui::Checkbox("Enabled", &enabled);
				ImGui::SliderFloat3("Position", position, -100, 100);
				ImGui::SliderFloat3("Direction", direction, -1, 1);
		

				ImGui::Text("Diffuse:");ImGui::SameLine();ImGui::ColorEdit3("MyColor##1", (float*)&diffuse, misc_flags);
				ImGui::Text("Specular:");ImGui::SameLine();ImGui::ColorEdit3("MyColor##2", (float*)&specularColor, misc_flags);
				ImGui::Spacing();
				//ImGui::SliderFloat("Attenuation Values", &atten[0],)
				ImGui::SliderFloat4("Attenuation Values", atten, 0.000f, 0.750f, "%.4f", 10.0f);
				ImGui::SliderFloat("Specular Power", &specPow, 0, 200);
				//ImGui::SliderFloat("Spot Light Range", &spotPow, 0, 50);
				ImGui::SliderFloat("Spot Light Angle", &spotAngle, 0, 50);

				lightsInScene[selected].diffuse = XMFLOAT4(diffuse);
				lightsInScene[selected].specularColor = XMFLOAT4(specularColor);
				lightsInScene[selected].position = XMFLOAT3(position);
				lightsInScene[selected].direction = XMFLOAT3(direction);
				lightsInScene[selected].enabled = enabled;
				lightsInScene[selected].lightType = lightType;
				lightsInScene[selected].attenuationValues = XMFLOAT4(atten);
				lightsInScene[selected].specularPower = specPow;
				lightsInScene[selected].spotPower = spotPow;
				lightsInScene[selected].spotAngle = spotAngle;

				//LightRepresent setting position and direction for shadow map usage
				lightRepresent[selected]->setDirection(direction[0], direction[1], direction[2]);
				lightRepresent[selected]->setPosition(position[0], position[1], position[2]);
			}
		}
			if (ImGui::TreeNode("Tessellation/Terrain Manipulation Settings"))
			{

				if (ImGui::Button("See Cool Visual Heightmap Steps"))
				{
					//11, 100, 0.10558, 0.99910
					qtsParameters[0] = 40;
					qtsParameters[1] = 100;
					qtsParameters[2] = 0.10558;
					qtsParameters[3] = 0.99910;
				}
				if (ImGui::Button("See Cool Visual Heightmap Steps 2"))
				{
					//11, 100, 0.10558, 0.99910
					qtsParameters[0] = 20;
					qtsParameters[1] = 100;
					qtsParameters[2] = 0.00015;
					qtsParameters[3] = 1.00010;
				}
				if (ImGui::Button("Reset"))
				{
					qtsParameters[0] = 25.0f;
					qtsParameters[1] = 25.0f;
					qtsParameters[2] = 1.0f;
					qtsParameters[3] = 1.01f;
				
				}
				ImGui::Text("Terrain Manipulation");
				ImGui::SliderFloat("Amplitude", &qtsParameters[0], 0, 100);
				ImGui::SliderFloat("Height Scale", &qtsParameters[1], 0, 100);
				ImGui::SliderFloat("World Step Res", &qtsParameters[2], 0, 3, "%.5f", 3);
				ImGui::SliderFloat("Offset Res", &qtsParameters[3], 1.0f, 1.05f, "%.5f", 3);

				ImGui::Text("Terrain Based Tessellation");
				ImGui::SliderFloat("Steepness Scale", &steepness, 0, 10);
				ImGui::Text("Camera Based Tessellation");
				ImGui::SliderFloat3("Distance Attenuation", atten, 0, 0.5, "%.5f", 1.6);
				ImGui::SliderFloat2("MinMaxLOD", minMaxLOD, 1, 10);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Post Processing Settings"))
			{
				ImGui::SliderInt("Post Process", &postProcess, 0, 2);
				std::string postProcessString;
				switch (postProcess)
				{
				case 0:
					postProcessString = "None";
					break;
				case 1:
					postProcessString = "Kernal";
					break;
				case 2:
					postProcessString = "Directional Blur";
					break;
				default:
					postProcessString = "None";
					break;
				}
				ImGui::Text(postProcessString.c_str());
				if (postProcess == 1)
				{
					ImGui::SliderFloat("Horizontal Texel", &horizontalTexel, 0.001f, 1.0f);
					ImGui::SliderInt("Filter", &filterType, 0, 8);
					std::string filterName;
					switch (filterType)
					{
					case 0:
						filterName = "Identity";
						break;
					case 1:
						filterName = "Edge Detection 1";
						break;
					case 2:
						filterName = "Edge Detection 2";
						break;
					case 3:
						filterName = "Edge Detection 3";
						break;
					case 4:
						filterName = "Sharpen";
						break;
					case 5:
						filterName = "Box Blur";
						break;
					case 6:
						filterName = "Guassian 3x3 Blur";
						break;
					case 7:
						filterName = "Emboss";
						break;
					case 8:
						filterName = "Variable Size Gaussian Blur";
						break;
					default:
						filterName = "None";
						break;
					}
					ImGui::Text(filterName.c_str());
					if (filterType == 8)
					{
						ImGui::SliderInt2("Texel Size", gDist, 1, 15);

					}
				}
				else if (postProcess == 2) {
					ImGui::SliderFloat2("Directional", directional, -1, 1);
					ImGui::SliderFloat("Directional Strenght", &dirStrength, 0, 200);
					ImGui::SliderInt("Directional Sample", &dirSample, 0, 50);
					ImGui::SliderFloat("Drag Amount", &dragAmount, 0, 5);
				}




		


				ImGui::TreePop();
			}



	}

	if (normalView)
	{
		quadExtra = 15;
	}
	else if (terrainShadows) {
		quadExtra = 0;
	}
	else {
		quadExtra = 0;
	}
	//ImGui::ShowDemoWindow();
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

