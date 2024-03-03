Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float screenHeight;
    float texel;
    float padding;
};
cbuffer Weighting : register(b1)
{
    float2 weightDirection;
    float strength;
    int sampleAmount;
    float dragAmount;
    float3 buffer;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2,weight3, weight4;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
	weight0 = 0.4062f;
    weight1 = 0.2442f;
    weight2 = 0.0545f;
    //weight0 = 0.382928;
    weight1 = 0.241732;
    weight2 = 0.060598;
    weight3 = 0.005977;
    weight4 = 0.000229;
	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSizeX = texel / screenWidth;
    float texelSizeY = texel / screenHeight;
    
    if (dragAmount <= 0.001f)
    {
        for (int i = 0; i < sampleAmount; i++)
        {
            colour += shaderTexture.Sample(SampleType, input.tex + ((float2(i * weightDirection.x * texelSizeX, i * weightDirection.y * texelSizeY) * strength)));
        }
    }
    else
    {
        for (int i = 0; i < sampleAmount; i++)
        {
            colour += shaderTexture.Sample(SampleType, input.tex + 
            ((float2(i * weightDirection.x * texelSizeX, i * weightDirection.y * texelSizeY) * strength) 
            * (i * dragAmount)));
        }
    }

    colour /= sampleAmount;
    colour.a = 1.0f;

    return colour;
}
