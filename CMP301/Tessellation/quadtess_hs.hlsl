// Tessellation Hull Shader
// Prepares control points for tessellation
#include "Tessellation.hlsli"
#define CONTROLPOINTS 6
cbuffer TessellationFactors : register(b0)
{
    float4 cameraPosition;
    float2 minMaxLOD;
    float2 padding;
    float3 atten;
    float steepnessScale;
}

    struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, CONTROLPOINTS> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    
    float3 x0 = float3(inputPatch[2].position.x, inputPatch[2].position.y, inputPatch[2].position.z);
    float3 x1 = float3(inputPatch[0].position.x, inputPatch[0].position.y, inputPatch[0].position.z);
    float3 x2 = float3(inputPatch[4].position.x, inputPatch[4].position.y, inputPatch[4].position.z);
    float3 x3 = float3(inputPatch[5].position.x, inputPatch[5].position.y, inputPatch[5].position.z);
 
    
    float3 edgeMidPoints[] = 
    {
        abs(lerp(x0, x1, 0.5)),//left
       abs(lerp(x1, x2, 0.5)),//bottom
        abs(lerp(x2, x3, 0.5)),//right
         abs(lerp(x0, x3, 0.5)) //top

    };
    float LOD0 = ComputePatchLOD(edgeMidPoints[0], cameraPosition.xyz, minMaxLOD.x, minMaxLOD.y, atten);
    float LOD1 = ComputePatchLOD(edgeMidPoints[1], cameraPosition.xyz, minMaxLOD.x, minMaxLOD.y, atten);
    float LOD2 = ComputePatchLOD(edgeMidPoints[2], cameraPosition.xyz, minMaxLOD.x, minMaxLOD.y, atten);
    float LOD3 = ComputePatchLOD(edgeMidPoints[3], cameraPosition.xyz, minMaxLOD.x, minMaxLOD.y, atten);
    float LODI = (LOD0 + LOD1 + LOD2 + LOD3) / 4.0f;
    
    float SLOD0 = abs(inputPatch[2].position.y - inputPatch[0].position.y);
    float SLOD1 = abs(inputPatch[0].position.y - inputPatch[4].position.y);
    float SLOD2 = abs(inputPatch[4].position.y - inputPatch[5].position.y);
    float SLOD3 = abs(inputPatch[2].position.y - inputPatch[5].position.y);
    float SLODI = (SLOD0 + SLOD1 + SLOD2 + SLOD3) / 4.0f;
    

   // float steepness = abs(maxSteepness - minSteepness) * steepnessScale;
    output.edges[0] = LOD0 + (SLOD0 * steepnessScale); //Left
    output.edges[1] = LOD1 + (SLOD1 * steepnessScale); //Bottom
    output.edges[2] = LOD2 + (SLOD2 * steepnessScale); //Right
    output.edges[3] = LOD3 + (SLOD3 * steepnessScale); //Tip

    output.inside[0] = 
    output.inside[1] = LODI + (SLODI * steepnessScale);
    
    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(6)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, CONTROLPOINTS> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;
    
    output.position = patch[pointId].position;
    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;

    return output;
}