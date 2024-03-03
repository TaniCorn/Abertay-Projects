 // Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)
#define MAX_LIGHTS 8
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
struct Light
{
    float4 diffuse;
    float4 specularColor;
    float3 position; //last element useless
    int lightType;
    float3 direction; //last element useless
    bool enabled;
    float4 attenuationValues; //l, a1, a2, a3
    float spotPower;
    float specularPower;
    float2 buffer;
};
cbuffer Lights : register(b0)
{
    float4 ambientLight;
    Light lights[MAX_LIGHTS];
}
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
    float3 cameraPosition : TEXCOORD2;
};
float4 calculateDirectionalLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, -lightDirection.xyz));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}
float4 calculatePointLighting(float3 lightVec, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightVec));
	float4 colour = saturate(ldiffuse * intensity);
	return colour;
}

float calculateSpecularPhong(float3 lightVec, float3 normal, float3 cameraVec, float4 specularColour, float specularPower)
{
    float lightIntensity = dot(lightVec, normal);
    float camIntensity = dot(cameraVec, normal);
    float specIntensity = pow(lightIntensity * camIntensity, specularPower);
    float4 color = specularColour * specIntensity;
    return color;

}

float4 calculateSpecularBlinnPhong(float3 lightVec, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    float3 halfway = normalize(lightVec + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);

}


float4 CalculateDirectionalLight(Light light, float3 normal)
{
    float4 result;
    result = calculateDirectionalLighting(light.direction, normal, light.diffuse);
    return result;  
}
float4 CalculatePointLight(Light light, float3 normal, float3 worldPosition)
{
    float4 result;
    float3 lightVector = normalize(light.position.xyz - worldPosition);
    result = calculatePointLighting(lightVector, normal, light.diffuse);
    return result;
}

float4 CalculateSpotLight(Light light, float3 normal, float3 worldPosition)
{
    float lightMag = distance(worldPosition.xyz, light.position.xyz);
    float3 lightVector = (light.position.xyz - worldPosition);
    float3 lightNormVec = normalize(lightVector);
    float diffuseIntensity = saturate(dot(normal, lightNormVec));

    float angleMult = pow(max(dot(-light.direction.xyz, lightNormVec), 0), light.spotPower /*Angle*/);

    diffuseIntensity = angleMult;

    float4 colour = saturate(light.diffuse * diffuseIntensity);
    
    return colour;
}
float4 CalculateSpecularLight(Light light, float3 normal, float3 viewVector,float3 worldPosition)
{
    float3 lightVector = normalize(light.position.xyz - worldPosition.xyz);
    float4 result = calculateSpecularBlinnPhong(lightVector, normal, viewVector, light.diffuse, light.specularPower);
    return result;

}
float CalculateAttenuation(Light light, float3 worldPosition)
{
    float lightMag;
    lightMag = distance(worldPosition, light.position.xyz);
    float attenuation = 1 / (light.attenuationValues.x /*x*/ + (light.attenuationValues.y /*y*/ * lightMag) + (light.attenuationValues.z /*z*/ * pow(lightMag, 2)));
    return attenuation;
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
    textureColour = texture0.Sample(sampler0, input.tex);
    float4 lightColour;
    //https://www.3dgep.com/texturing-lighting-directx-11/#Struct_Packing
    //I referenced this site when designing the code below and the cbuffer and light struct.

    float4 result = { 0, 0, 0, 0 };
    //Calculate all lights effect for pixel
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (!lights[i].enabled)
        {
            continue;
        }
        
        float4 lightResult = { 0, 0, 0, 0 };
        switch (lights[i].lightType)
        {
            case 0: //Directional Light
                lightResult = CalculateDirectionalLight(lights[i], input.normal);
                break;
            case 1: //Point Light
                lightResult = CalculatePointLight(lights[i], input.normal, input.worldPosition);
                lightResult += CalculateSpecularLight(lights[i], input.normal, input.cameraPosition, input.worldPosition);
                lightResult *= CalculateAttenuation(lights[i], input.worldPosition);
                break;
            case 2: //Spot Light
                lightResult = CalculateSpotLight(lights[i], input.normal, input.worldPosition);
                lightResult += CalculateSpecularLight(lights[i], input.normal, input.cameraPosition, input.worldPosition);
                 lightResult *= CalculateAttenuation(lights[i], input.worldPosition);
                break;
            default:
                break;
        }

        result += lightResult;

    }
    return (result * textureColour); // + specularColour;
}
