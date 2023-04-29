// Horizontal blur shader handler
// Loads horizontal blur shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class DirectionalBlur : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		float screenHeight;
		float texel;
		float padding;
	};
	struct WeightBufferType {
		XMFLOAT2 direction;
		float strength;
		int sampleAmount;
		float dragAmount;
		XMFLOAT3 buffer;
	};

public:

	DirectionalBlur(ID3D11Device* device, HWND hwnd);
	~DirectionalBlur();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT2 dimensions, float texel, XMFLOAT3 weights, int sam, float dragAmount);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* weightBuffer;
};