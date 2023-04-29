// Horizontal blur shader handler
// Loads horizontal blur shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class KernalFilter : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		XMFLOAT2 screenResolution;
		float texelSize;
		int filter;
		XMINT2 gDist;
		XMFLOAT2 padding;
	};

public:

	KernalFilter(ID3D11Device* device, HWND hwnd);
	~KernalFilter();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT2 resolution,XMINT2 gDist, float texel, int filter);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* weightBuffer;
};