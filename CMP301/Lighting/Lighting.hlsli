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
    float spotAngle;
    float buffer;
};

//Different Light Types
float PointFactor(float3 lightPosition, float3 worldPosition, float3 normal)
{
    float3 lightVector = normalize(lightPosition - worldPosition);
    float intensity = saturate(dot(normal, lightVector));
    return intensity;
}
float SpotFactor(float3 lightPosition, float3 worldPosition, float3 lightDirection, float angle)
{
    float3 lightVector = (lightPosition.xyz - worldPosition);
    float3 lightNormVec = normalize(lightVector);
    float intensity = pow(max(dot(-lightDirection.xyz, lightNormVec), 0), angle /*Angle*/);
    return intensity;
}
float DirectionalFactor(float3 lightDirection, float3 normal)
{
    float intensity = saturate(dot(normal, -lightDirection.xyz));
    return intensity;
}
//For now it is just the light color as we haven't implemented materials
float4 MaterialLightColor(float4 lightColor, float4 materialColor)
{
    float4 result = lightColor; //*materialcolor
    return saturate(result);
}

//Specular Models
float Phong_SpecularFactor(float3 lightPosition, float3 normal, float3 worldPosition, float3 cameraVec, float specularPower)
{
    float3 lightVector = normalize(lightPosition.xyz - worldPosition.xyz);
    
    float lightIntensity = dot(lightVector, normal);
    float camIntensity = dot(cameraVec, normal);
    
    float specIntensity = pow(lightIntensity * camIntensity, specularPower);
    return specIntensity;
}

float BlinnPhong_SpecularFactor(float3 lightPosition, float3 normal, float3 worldPosition, float3 viewVector, float specularPower)
{
    float3 lightVector = normalize(lightPosition.xyz - worldPosition.xyz);
    
    float3 halfway = normalize(lightVector + viewVector);
    
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return specularIntensity;
}

//Attenuation
float4 ApplyAttenuation(float3 attenuationParameters, float4 lightColor, float3 lightPosition, float3 worldPosition)
{
    float lightMag = distance(worldPosition, lightPosition.xyz);
    float4 attenuation = lightColor / (attenuationParameters.x /*x*/ + (attenuationParameters.y /*y*/ * lightMag) + (attenuationParameters.z /*z*/ * pow(lightMag, 2)));
    return attenuation;
}




#define MAX_LIGHTS 8
float4 CalculateLight(Light lights[MAX_LIGHTS], float3 normal, float3 worldPosition, float3 cameraPosition, float4 ambient)
{
    
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
        
        float4 lightResult = float4(0, 0, 0, 0);
        float4 diffuseLight = float4(0, 0, 0, 0);
        float4 specularLight = float4(0, 0, 0, 0);
        float diffuseFactor = 0;
        float specularFactor = 0;
        
        diffuseLight = MaterialLightColor(lights[i].diffuse, float4(0, 0, 0, 0));
        specularLight = MaterialLightColor(lights[i].specularColor, float4(0, 0, 0, 0));

        switch (lights[i].lightType)
        {
            case 0: //Directional Light
                diffuseFactor = DirectionalFactor(lights[i].direction, normal);
                lightResult = saturate(diffuseLight * diffuseFactor);
                break;
            case 1: //Point Light
            
                diffuseFactor = PointFactor(lights[i].position, worldPosition, normal);
                specularFactor = BlinnPhong_SpecularFactor(lights[i].position, normal, worldPosition, cameraPosition, lights[i].specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights[i].specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights[i].diffuse.w));
                lightResult = ApplyAttenuation(lights[i].attenuationValues.xyz, lightResult, lights[i].position, worldPosition);
                break;
            case 2: //Spot Light
            
                diffuseFactor = SpotFactor(lights[i].position, worldPosition, lights[i].direction, lights[i].spotAngle);
                specularFactor = BlinnPhong_SpecularFactor(lights[i].position, normal, worldPosition, cameraPosition, lights[i].specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights[i].specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights[i].diffuse.w));
                lightResult = ApplyAttenuation(lights[i].attenuationValues.xyz, lightResult, lights[i].position, worldPosition);
                break;
            default:
                break;
        }

        result += lightResult;

    }
    return (result + ambient);
}
float4 CalculateLight(Light lights, float3 normal, float3 worldPosition, float3 cameraPosition, float4 ambient)
{
    
    float4 lightColour;
    //https://www.3dgep.com/texturing-lighting-directx-11/#Struct_Packing
    //I referenced this site when designing the code below and the cbuffer and light struct.

    float4 result = { 0, 0, 0, 0 };
    //Calculate all lights effect for pixel
        if (!lights.enabled)
        {
            return float4(0, 0, 0, 0);
        }
        
        float4 lightResult = float4(0, 0, 0, 0);
        float4 diffuseLight = float4(0, 0, 0, 0);
        float4 specularLight = float4(0, 0, 0, 0);
        float diffuseFactor = 0;
        float specularFactor = 0;
        
        diffuseLight = MaterialLightColor(lights.diffuse, float4(0, 0, 0, 0));
        specularLight = MaterialLightColor(lights.specularColor, float4(0, 0, 0, 0));

        switch (lights.lightType)
        {
            case 0: //Directional Light
                diffuseFactor = DirectionalFactor(lights.direction, normal);
                lightResult = saturate(diffuseLight * diffuseFactor);
                break;
            case 1: //Point Light
            
                diffuseFactor = PointFactor(lights.position, worldPosition, normal);
                specularFactor = BlinnPhong_SpecularFactor(lights.position, normal, worldPosition, cameraPosition, lights.specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights.specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights.diffuse.w));
                lightResult = ApplyAttenuation(lights.attenuationValues.xyz, lightResult, lights.position, worldPosition);
                break;
            case 2: //Spot Light
            
                diffuseFactor = SpotFactor(lights.position, worldPosition, lights.direction, lights.spotAngle);
                specularFactor = BlinnPhong_SpecularFactor(lights.position, normal, worldPosition, cameraPosition, lights.specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights.specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights.diffuse.w));
                lightResult = ApplyAttenuation(lights.attenuationValues.xyz, lightResult, lights.position, worldPosition);
                break;
            default:
                break;
        }

        result += lightResult;

    return (result + ambient);
}

///////Shadows

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow( Texture2D sMap,  SamplerState shadowSampler,  float2 uv,  float4 lightViewPosition,  float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords( float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 LightAndShadowCalculation(Texture2D depthMapTexture, Light lights, float4 lightViewPos, SamplerState shadowSampler, float3 normal, float3 worldPosition, float3 cameraPosition, float shadowMapBias, float4 ambient)
{
    float4 result = { 0, 0, 0, 0.0f };
        
        if (!lights.enabled)
        {
        return float4(0, 0, 0, 0);
    }
        
        float4 lightResult = float4(0, 0, 0, 0);
        float4 diffuseLight = float4(0, 0, 0, 0);
        float4 specularLight = float4(0, 0, 0, 0);
        float diffuseFactor = 0;
        float specularFactor = 0;
        
        diffuseLight = MaterialLightColor(lights.diffuse, float4(0, 0, 0, 0));
        specularLight = MaterialLightColor(lights.specularColor, float4(0, 0, 0, 0));
        
        
        switch (lights.lightType)
        {
            case 0: //Directional Light
                float2 pTexCoord = getProjectiveCoords(lightViewPos);
                if (hasDepthData(pTexCoord))
                {
                    if (!isInShadow(depthMapTexture, shadowSampler, pTexCoord, lightViewPos, shadowMapBias))
                    {
                    // is NOT in shadow, therefore light
                        diffuseFactor = DirectionalFactor(lights.direction, normal);
                        lightResult = saturate(diffuseLight * diffuseFactor);
                    }
                } 
        
                break;
            case 1: //Point Light
            
                diffuseFactor = PointFactor(lights.position, worldPosition, normal);
                specularFactor = BlinnPhong_SpecularFactor(lights.position, normal, worldPosition, cameraPosition, lights.specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights.specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights.diffuse.w));
                lightResult = ApplyAttenuation(lights.attenuationValues.xyz, lightResult, lights.position, worldPosition);
                break;
            case 2: //Spot Light
            
                diffuseFactor = SpotFactor(lights.position, worldPosition, lights.direction, lights.spotAngle);
                specularFactor = BlinnPhong_SpecularFactor(lights.position, normal, worldPosition, cameraPosition, lights.specularPower);
                lightResult = (clamp((specularFactor * specularLight), 0, lights.specularColor.w) + clamp((diffuseLight * diffuseFactor), 0, lights.diffuse.w));
                lightResult = ApplyAttenuation(lights.attenuationValues.xyz, lightResult, lights.position, worldPosition);
                break;
            default:
                break;
        }

        result += lightResult;

    return (result + ambient);
}

float ShaderModel2(float intensity)
{
    
    float finalIntensity = intensity;
    if (finalIntensity > 0.75f)
    {
        finalIntensity = 1.0f;
    }
    else if (finalIntensity > 0.5f)
    {
        finalIntensity = 0.75f;
    }
    else if (finalIntensity > 0.25f)
    {
        finalIntensity = 0.5f;
    }
    else if (finalIntensity > 0.01f)
    {
        finalIntensity = 0.25f;
    }
    else
    {
        finalIntensity = 0;
    }
        return finalIntensity;
}