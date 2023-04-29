// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.
#include "Tessellation.hlsli"
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
cbuffer ParamTypes : register(b0)
{
    float amplitude;
    float stepres;
    float offsetRes;
    float scale;
    float4 depthOn;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};




    OutputType main(InputType input)
{
    OutputType output;

	 // Pass the vertex position into the hull shader.
    input.position.y = GetHeight(input.tex, texture0, sampler0) * amplitude;
    output.position = input.position;

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = input.normal;
    return output;
}
