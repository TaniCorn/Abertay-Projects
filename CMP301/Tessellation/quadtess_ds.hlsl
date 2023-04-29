// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
#define CONTROLPOINTS 6
#include "Lighting.hlsli"
#include "Tessellation.hlsli"
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer CameraBuffer : register(b1)
{
    float3 cameraPos;
    float padding;
};
cbuffer ParamTypes : register(b2)
{
    float amplitude;
    float stepres;
    float offsetRes;
    float scale;
    float4 depthOn;
};
cbuffer ShadowBuffer : register(b3)
{
    matrix lightViewMatrix[MAX_LIGHTS];
    matrix lightProjectionMatrix[MAX_LIGHTS];
}
struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 cameraPosition : TEXCOORD2;
    float4 depthPosition : TEXCOORD3;
    float4 lightViewPos[MAX_LIGHTS] : TEXCOORD4;
};



[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, CONTROLPOINTS> patch)
{
    float3 vertexPosition, normal;
    float2 tex;
    OutputType output;
 

    float3 v1 = lerp(patch[0].position, patch[2].position, uvwCoord.y);
    float3 v2 = lerp(patch[4].position, patch[5].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    float3 n1 = lerp(patch[0].normal, patch[2].normal, uvwCoord.y);
    float3 n2 = lerp(patch[4].normal, patch[5].normal, uvwCoord.y);
    normal = lerp(n1, n2, uvwCoord.x);
    
    float2 t1 = lerp(patch[0].tex, patch[2].tex, uvwCoord.y);
    float2 t2 = lerp(patch[4].tex, patch[5].tex, uvwCoord.y);
    tex = lerp(t1, t2, uvwCoord.x);
    
    //vertexPosition.z += sin(uvwCoord.x * frequency + (time * scale)) * amplitude;
    // Calculate the position of the new vertex against the world, view, and projection matrices.

    // Send the input color into the pixel shader.
    //output.tex = uvwCoord;
    
    
    vertexPosition.y = GetHeight(tex, texture0, sampler0) * amplitude;
    

    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.normal = normal;
    output.tex = tex;
    
    output.worldPosition = mul(float4(vertexPosition, 1), worldMatrix).xyz;
    output.cameraPosition = cameraPos.xyz - output.worldPosition.xyz;
    output.cameraPosition = normalize(output.cameraPosition);
    
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    
    }
    output.depthPosition = output.position;
    return output;
}

