#pragma once

#include "DXF.h"
#include "MultiLightShader.h"

using namespace std;
using namespace DirectX;

class FrankLunaShader : public MultiLightShader
{
public:

public:
	FrankLunaShader(ID3D11Device* device, HWND hwnd);
	~FrankLunaShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[MAX_LIGHTS],LightData lights[MAX_LIGHTS], XMMATRIX lightProjection[MAX_LIGHTS], XMMATRIX lightView[MAX_LIGHTS], XMFLOAT3 cameraPosition, float shadowBias, int lightingModel);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11SamplerState* sampleState;
};

