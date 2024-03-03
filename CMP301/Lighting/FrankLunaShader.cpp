#include "FrankLunaShader.h"

FrankLunaShader::FrankLunaShader(ID3D11Device* device, HWND hwnd) : MultiLightShader(device, hwnd)
{
	initShader(L"light_vs.cso", L"FrankLunaLight_ps.cso");
}


FrankLunaShader::~FrankLunaShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	MultiLightShader::~MultiLightShader();
}

void FrankLunaShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
	initBasicBuffers();

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}


void FrankLunaShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, 
	ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[MAX_LIGHTS], LightData lights[MAX_LIGHTS], XMMATRIX lightProjection[MAX_LIGHTS], XMMATRIX lightView[MAX_LIGHTS], 
	XMFLOAT3 cameraPosition, float shadowBias, int lightingModel)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	initBasicShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lights, lightProjection, lightView, cameraPosition, shadowBias, lightingModel);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);
	deviceContext->VSSetConstantBuffers(2, 1, &shadowBuffer);
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, MAX_LIGHTS, depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowSampler);
}
