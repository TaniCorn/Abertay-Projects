#include "TM_InterfacePERLIN.h"

void TM_InterfacePERLIN::HandleGUI()
{
	ImGui::Checkbox("Auto Scroll X", &perlinParams.autoScrollX);
	if (perlinParams.autoScrollX) {
		update = true;
		perlinParams.xDisplacement += 0.05f;
	}
	ImGui::Checkbox("Auto Scroll Z", &perlinParams.autoScrollZ);
	if (perlinParams.autoScrollZ) {
		update = true;
		perlinParams.yDisplacement += 0.05f;
	}
	if (!warp)
	{
		if (ImGui::TreeNode("Perlin")) {
			ValueChange("xFrequency", perlinParams.xFrequency, 0, 10);
			ValueChange("yFrequency", perlinParams.yFrequency, 0, 10);
			ValueChange("xdisplacement", perlinParams.xDisplacement, 0.0f, 5);
			ValueChange("zdisplacement", perlinParams.yDisplacement, 0.0f, 5);
			ValueChange("amplitude", perlinParams.amplitude, -25.0f, 25.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Fractal Perlin"))
		{
			ValueChange("fractal", perlinParams.fractal, 0, 10);
			ValueChange("frequency multiplier", perlinParams.frequencyMultiplier, 0, 10);
			ValueChange("amplitude multiplier", perlinParams.amplitudeMultiplier, 0, 10);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Perlin Overlay"))
		{
			ImGui::Checkbox("Enable Overlay", &perlinParams.warping);

			ValueChange("Warped X Displacement", perlinParams.warpedXDisplacement, 0, 2);
			ValueChange("Warped Y Displacement", perlinParams.warpedYDisplacement, 0, 2);

			ImGui::TreePop();
		}

	}
	else {
			ValueChange("xFrequency", perlinParams.xFrequency, 0, 10);
			ValueChange("yFrequency", perlinParams.yFrequency, 0, 10);

			ValueChange("amplitude", perlinParams.amplitude, -25.0f, 100.0f);
			/*ValueChange("warp amplitude", perlinParams.warpAmp, -5.0f, 5.0f);
			ValueChange("fbm amplitude", perlinParams.fbmAmp, -5.0f, 5.0f);
			ValueChange("frequency multiplier", perlinParams.frequencyMultiplier, 0, 10);
			ValueChange("amplitude multiplier", perlinParams.amplitudeMultiplier, 0, 10);*/

			ValueChange("fractal", perlinParams.fractal, 0, 10);

			ValueChange("xdisplacement", perlinParams.xDisplacement, 0.0f, 5);
			ValueChange("zdisplacement", perlinParams.yDisplacement, 0.0f, 5);
			ValueChange("Warped X Displacement", perlinParams.warpedXDisplacement, 0, 2);
			ValueChange("Warped Y Displacement", perlinParams.warpedYDisplacement, 0, 2);

	}



}



void TM_InterfacePERLIN::ApplyFunction(int resolution, float* hMap, PerlinParams params)
{
	float height;

	TM_InterfaceMASTER::FlattenHeightMap(hMap, resolution);
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			double xAxis = (((double)i / resolution) * params.xFrequency) + params.xDisplacement;
			double yAxis = (((double)j / resolution) * params.yFrequency) + params.yDisplacement;
			height = PerlinNoise::PerlinNoise::noise(xAxis, yAxis) * params.amplitude;
			//height = PerlinNoise::PerlinNoise::noise01(xDisplacement + i, yDisplacement + j) * amplitude;//perlin noise but janky
			//height = PerlinNoise::PerlinNoise::grad(i, xparam, yparam); //Generates the vectors that we talked about. 
			//height = PerlinNoise::PerlinNoise::fade(i); //No idea
			//height = PerlinNoise::PerlinNoise::fadeDerivative(i); //No idea

			hMap[(j * resolution) + i] = height;
		}
	}

}

void TM_InterfacePERLIN::FractalFunction(int resolution, float* hMap, PerlinParams params) {

	params.warpFunc = false;
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			float height = 0;
			float xFrequency = params.xFrequency;
			float yFrequency = params.yFrequency;
			float amplitude = params.amplitude;
			for (int k = 0; k < params.fractal; k++)
			{
				double xAxis, yAxis;
				if (params.warping)
				{
					yAxis = (((double)j / resolution) * yFrequency) + NoiseSample(resolution, i, j, xFrequency, yFrequency, params.warpedXDisplacement, params.warpedYDisplacement) + params.xDisplacement;
					xAxis = (((double)i / resolution) * xFrequency) + NoiseSample(resolution, i, j, xFrequency, yFrequency, params.warpedXDisplacement, params.warpedYDisplacement) + params.yDisplacement;
				}
				else {
					yAxis = (((double)j / resolution) * yFrequency) + params.yDisplacement;
					xAxis = (((double)i / resolution) * xFrequency) + params.xDisplacement;

				}
				height += PerlinNoise::PerlinNoise::noise(xAxis, yAxis) * amplitude;

				xFrequency *= params.frequencyMultiplier;
				yFrequency *= params.frequencyMultiplier;

				amplitude *= params.amplitudeMultiplier;
			}
				hMap[(j * resolution) + i] = height;
		}
	}

}

void TM_InterfacePERLIN::WarpedFunction(int resolution, float* hMap, PerlinParams params)
{
	params.warpFunc = true;
	XMFLOAT2 d1 = XMFLOAT2(params.xDisplacement, params.yDisplacement);
	XMFLOAT2 d2 = XMFLOAT2(params.warpedXDisplacement, params.warpedYDisplacement);
	//These don't really make a big enough impact for me to conisder them
	XMFLOAT2 d3 = XMFLOAT2(0,0);
	XMFLOAT2 d4 = XMFLOAT2(0,0);
	float warpAmp = 1;


	srand(TM_InterfaceMASTER::seed);
	float offset = rand();
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {

			XMFLOAT2 point;
			point.y = (((double)j / resolution) * params.yFrequency);
			point.x = (((double)i / resolution) * params.xFrequency);
			XMFLOAT2 q = XMFLOAT2(
				FBMFunction(XMFLOAT2(point.x +d1.x, point.y + d1.y), params),
				FBMFunction(XMFLOAT2(point.x + d2.x, point.y + d2.y), params)
			);

			//float height = FBMFunction(XMFLOAT2(point.x + (params.warpAmp * q.x), point.y + (params.warpAmp * q.y)), params);//Single Warp

			XMFLOAT2 r = XMFLOAT2(
				FBMFunction(XMFLOAT2(point.x + (warpAmp*q.x) + d3.x, point.y + (warpAmp *q.y) + d3.y), params),
				FBMFunction(XMFLOAT2(point.x + (warpAmp * q.x) + d4.x, point.y + (warpAmp * q.y) + d4.y), params)
			);

			float height = FBMFunction(XMFLOAT2(point.x + (params.warpAmp * r.x), point.y + (params.warpAmp * r.y)), params) * params.amplitude;
			hMap[(j * resolution) + i] = height;
		}
	}
}

float TM_InterfacePERLIN::NoiseSample(int resolution, int x, int y, int xFrequency, int yFrequency, float xDisplacement, float yDisplacement)
{
	double xAxis = (((double)x / resolution) * xFrequency) + xDisplacement;
	double yAxis = (((double)y / resolution) * yFrequency) + yDisplacement;
	return PerlinNoise::PerlinNoise::noise(xAxis, yAxis);
}

float TM_InterfacePERLIN::FBMFunction(double xAx, double yAx, PerlinParams params)
{
	float height = 0;
	float xFrequency = params.xFrequency;
	float yFrequency = params.yFrequency;
	float amplitude =params.fbmAmp;
	for (int k = 0; k < params.fractal; k++)
	{
		double xAxis, yAxis;

		yAxis = yAx;
		xAxis = xAx;

		height += PerlinNoise::PerlinNoise::noise(xAxis, yAxis) * amplitude;

		xFrequency *= params.frequencyMultiplier;
		yFrequency *= params.frequencyMultiplier;

		amplitude *= params.amplitudeMultiplier;
	}
	return height;
}

float TM_InterfacePERLIN::FBMFunction(XMFLOAT2 point, PerlinParams params)
{
	float height = 0;
	float xFrequency = params.xFrequency;
	float yFrequency = params.yFrequency;
	float amplitude = params.fbmAmp;
	for (int k = 0; k < params.fractal; k++)
	{


		height += PerlinNoise::PerlinNoise::noise(point.x, point.y) * amplitude;

		xFrequency *= params.frequencyMultiplier;
		yFrequency *= params.frequencyMultiplier;

		amplitude *= params.amplitudeMultiplier;
	}
	return height;
}
