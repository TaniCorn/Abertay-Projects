#include "QuadTessShader.h"
// tessellation shader.cpp
#include "QuadTessShader.h"


QuadTessShader::QuadTessShader(ID3D11Device* device, HWND hwnd) : MultiLightShader(device, hwnd)
{
	initShader(L"quadtess_vs.cso", L"quadtess_hs.cso", L"quadtess_ds.cso", L"quadtess_ps.cso");
}


QuadTessShader::~QuadTessShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	MultiLightShader::~MultiLightShader();

}

void QuadTessShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
	initBasicBuffers();
	D3D11_SAMPLER_DESC samplerDesc;
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
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.


	D3D11_BUFFER_DESC tessFactorDesc;
	tessFactorDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessFactorDesc.ByteWidth = sizeof(TessFactorType);
	tessFactorDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessFactorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessFactorDesc.MiscFlags = 0;
	tessFactorDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tessFactorDesc, NULL, &tessFactorBuffer);
	
	tessFactorDesc.ByteWidth = sizeof(HeightMapType);

	renderer->CreateBuffer(&tessFactorDesc, NULL, &heightMapBuffer);


}

void QuadTessShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void QuadTessShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, 
	ID3D11ShaderResourceView* heightmap, ID3D11ShaderResourceView* planeTexture, XMFLOAT2 minMaxLOD, XMFLOAT4 heightmapParams,float steepness,XMFLOAT3 atten, 
	LightData lights[MAX_LIGHTS], ID3D11ShaderResourceView* depthMap[MAX_LIGHTS], XMMATRIX	lightProjection[MAX_LIGHTS], XMMATRIX lightView[MAX_LIGHTS], 
	XMFLOAT3 cameraPosition, float shadowBias, int lightingModel, float extra)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	initBasicShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lights,lightProjection , lightView, cameraPosition, shadowBias, lightingModel);



	result = deviceContext->Map(tessFactorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TessFactorType* tessPtr = (TessFactorType*)mappedResource.pData;
	tessPtr->cameraPosition = XMFLOAT4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1);
	tessPtr->minMaxLOD.x = minMaxLOD.x;
	tessPtr->minMaxLOD.y = minMaxLOD.y;
	tessPtr->atten = atten;
	tessPtr->steepnessScale = steepness;
	deviceContext->Unmap(tessFactorBuffer, 0);

	result = deviceContext->Map(heightMapBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HeightMapType* heightMap = (HeightMapType*)mappedResource.pData;
	heightMap->amplitude = heightmapParams.x;
	heightMap->scale = heightmapParams.y;
	heightMap->stepres = heightmapParams.z;
	heightMap->offsetRes = heightmapParams.w;
	heightMap->depthOn = XMFLOAT4(extra, extra, extra, extra);
	deviceContext->Unmap(heightMapBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &heightMapBuffer);
	deviceContext->VSSetShaderResources(0, 1, &heightmap);
	deviceContext->VSSetSamplers(0, 1, &sampleState);


	deviceContext->HSSetConstantBuffers(0, 1, &tessFactorBuffer);

	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &cameraBuffer);
	deviceContext->DSSetConstantBuffers(2, 1, &heightMapBuffer);
	deviceContext->DSSetConstantBuffers(3, 1, &shadowBuffer);
	deviceContext->DSSetShaderResources(0, 1, &heightmap);
	deviceContext->DSSetSamplers(0, 1, &sampleState);


	deviceContext->PSSetConstantBuffers(0, 1, &heightMapBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &lightBuffer);
	deviceContext->PSSetShaderResources(0, 1, &heightmap);
	deviceContext->PSSetShaderResources(1, 1, &planeTexture);
	deviceContext->PSSetShaderResources(2, MAX_LIGHTS, depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowSampler);

}
