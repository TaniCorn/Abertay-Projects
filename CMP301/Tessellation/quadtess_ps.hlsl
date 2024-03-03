// Tessellation pixel shader
// Output colour passed to stage.
#include "Tessellation.hlsli"
#include "Lighting.hlsli"
Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D shadowTextures[MAX_LIGHTS] : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);
cbuffer ParamTypes : register(b0)
{
    float amplitude;
    float stepres;
    float offsetRes;
    float scale;
    float4 depthOn;
};

cbuffer Lights : register(b1)
{
    float4 ambientLight;
    Light lights[MAX_LIGHTS];
    float shadowBias;
    int lightingModel;
    float2 buffer;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 cameraPosition : TEXCOORD2;
    float4 depthPosition : TEXCOORD3;
    float4 lightViewPos[MAX_LIGHTS] : TEXCOORD4;
};



float4 main(InputType input) : SV_TARGET
{
    
    input.normal = RecalculateNormals1(input.tex, offsetRes, stepres, scale, texture0, sampler0);
    if (depthOn.x > 10)
    {
        return float4(input.normal, 1);
    }
    else if (depthOn.x > 1)
    {
        float depthValue;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
        depthValue = input.depthPosition.z / input.depthPosition.w;
        return float4(depthValue, depthValue, depthValue, 1.0f);
    }

        float4 textureColour;
    textureColour = texture1.Sample(sampler0, input.tex);
    float4 lightColour;
    float4 result = float4(0, 0, 0, 0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lightingModel == 1)
        {
            float3 norm = floor(input.normal * 100.0f) / 100.0f;
            float3 worldpos = floor(input.worldPosition);
            result += LightAndShadowCalculation(shadowTextures[i], lights[i], input.lightViewPos[i], shadowSampler, norm, worldpos, input.cameraPosition, shadowBias, ambientLight);
        }
        else
        {
            result += LightAndShadowCalculation(shadowTextures[i], lights[i], input.lightViewPos[i], shadowSampler, input.normal, input.worldPosition, input.cameraPosition, shadowBias, ambientLight);
            
        }
    }
    
    if (lightingModel == 2)
    {
        result = float4(ShaderModel2(result.x), ShaderModel2(result.y), ShaderModel2(result.z), ShaderModel2(result.w));

    }
    float4 endResult = saturate(result * textureColour);
    return endResult;

}