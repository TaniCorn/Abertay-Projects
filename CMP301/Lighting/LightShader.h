#pragma once

#include "DXF.h"
#include "MultiLightShader.h"

using namespace std;
using namespace DirectX;

/// <summary>
/// Although the shader functions do not use this, the structure of the code, in terms of max light and packing rules were influenced by this site
/// https://www.3dgep.com/texturing-lighting-directx-11/#Packing_Rules_for_Constant_Buffers
/// </summary>
class LightShader : public MultiLightShader
{
public:


public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, LightData lights[MAX_LIGHTS], XMFLOAT3 cameraPosition);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11SamplerState* sampleState;
};

