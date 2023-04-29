// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"
#include "MultiLightShader.h"
using namespace std;
using namespace DirectX;


class QuadTessShader : public MultiLightShader
{
private:

	struct TessFactorType {
		XMFLOAT4 cameraPosition;
		XMFLOAT2 minMaxLOD;
		XMFLOAT2 padding;
		XMFLOAT3 atten;
		float steepnessScale;
	};
	struct HeightMapType {
		float amplitude;
		float stepres;
		float offsetRes;
		float scale;
		XMFLOAT4 depthOn;
	};
public:

	QuadTessShader(ID3D11Device* device, HWND hwnd);
	~QuadTessShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
		ID3D11ShaderResourceView* heightmap, ID3D11ShaderResourceView* planeTexture, XMFLOAT2 minMaxLOD, XMFLOAT4 heightmapParams, float steepness, XMFLOAT3 atten,
		LightData lights[MAX_LIGHTS], ID3D11ShaderResourceView* depthMap[MAX_LIGHTS], XMMATRIX	lightProjection[MAX_LIGHTS], XMMATRIX lightView[MAX_LIGHTS], XMFLOAT3 cameraPosition, float shadowBias, int lightingModel, float extra);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* tessFactorBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* heightMapBuffer;
};
