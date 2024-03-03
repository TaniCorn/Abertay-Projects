 // Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)
#include "Lighting.hlsli"
#define MAX_LIGHTS 8
Texture2D texture0 : register(t0);
Texture2D shadowTextures[MAX_LIGHTS] : register(t1);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer Lights : register(b0)
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
    float4 lightViewPos[MAX_LIGHTS] : TEXCOORD3;
};




float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    textureColour = texture0.Sample(sampler0, input.tex);
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
