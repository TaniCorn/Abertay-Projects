//Samples the red channel
float GetHeight(float2 uv, Texture2D texture0, SamplerState sampler0)
{
    float4 map = texture0.SampleLevel(sampler0, uv, 0);
    return map.x;
}
///Samples the red channel
float GetHeight(float u, float v, Texture2D texture0, SamplerState sampler0)
{
    float4 map = texture0.SampleLevel(sampler0, float2(u, v), 0);
    return map.x;
}


float ComputeScaledDistance(float3 from, float3 to, float minDistance, float maxDistance)
{
    float d = distance(from, to);

	//minMaxDistance
    d = (d - minDistance) / (maxDistance - minDistance);
    return d;
}
float ComputeAttenuationDistance(float3 from, float3 to, float3 atten)
{
    float d = (distance(from, to));
    
    float attenuation = 1 / (atten.x /*x*/ + (atten.y /*y*/ * d) + (atten.z /*z*/ * pow(d, 2)));
    return clamp(attenuation, 0, 1);
}

float ComputePatchLOD(float3 pnt, float3 cameraPosition, float minLOD, float maxLOD, float3 atten)
{
    float dist = ComputeAttenuationDistance(cameraPosition, pnt, atten);
	
    dist = lerp(minLOD, maxLOD, dist);
    return dist;
}
float ComputeAttenuationDistance(float from, float to, float3 atten)
{
    float d = (distance(from, to));
    
    float attenuation = 1 / (atten.x /*x*/ + (atten.y /*y*/ * d) + (atten.z /*z*/ * pow(d, 2)));
    return clamp(attenuation, 0, 1);
}
float ComputePatchLOD(float ySpace, float yCamera, float minLOD, float maxLOD, float3 atten)
{
    float dist = ComputeAttenuationDistance(yCamera, ySpace, atten);
	
    dist = lerp(maxLOD, minLOD, 1.0f - dist);
    return dist;
}
float DistFromPointToLine(float2 x2, float2 x1, float2 x0)
{
    float top = length((x2.x - x1.x) * (x1.y - x0.y) - (x1.x - x0.x) * (x2.y - x1.y));
    float bottom = sqrt(pow((x2.x - x1.x), 2) + pow((x2.y - x1.y), 2));
    float dist = top / bottom;
    return abs(dist);

}

float3 RecalculateNormals1(float2 uv, float offsetRes, float stepres, float scale, Texture2D texture0, SamplerState sampler0)
{
   // float offset = offsetRes / 512;; //screenwidth/height//Have a lower res
    float offset = 1.0f + offsetRes; //Map resolution by texel count // Have a higher res
    
    float worldStep = stepres;
    float4 one = float4(1, 1, 1, 1);
    float4 origin = one;
    float4 north = one;
    north.z += worldStep;
    float4 east = one;
    east.x += worldStep;
    float4 south = one;
    south.z -= worldStep;
    float4 west = one;
    west.x -= worldStep;
    
    origin.y = GetHeight(uv.x, uv.y, texture0, sampler0) * scale; //pow(uv.x * 10, 3);
    north.y = GetHeight(uv.x, (uv.y + offset), texture0, sampler0) * scale; //pow(uv.y + offset * 10, 3);
    south.y = GetHeight(uv.x, (uv.y - offset), texture0, sampler0) * scale; //pow(uv.y + offset * 10, 3);
    east.y = GetHeight((uv.x + offset), uv.y, texture0, sampler0) * scale; //pow(uv.x + offset * 10, 3); 
    west.y = GetHeight((uv.x - offset), uv.y, texture0, sampler0) * scale; //pow(uv.x + offset * 10, 3); 
    
    float4 vec1 = (origin - east);
    float4 vec2 = (origin - north);
    float4 vec3 = (origin - west);
    float4 vec4 = (origin - south);
    
    float3 tan = normalize(-cross(vec1.xyz, vec2.xyz));
    float3 tan1 = normalize(-cross(vec3.xyz, vec4.xyz));
    float3 total = (tan1 + tan) / 2;
    return tan;

}