Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);
//Just remember everything must = 1
#define identity float3x3(0, 0, 0, 0, 1, 0, 0, 0, 0)
#define edge0 float3x3(1, 0, -1, 0, 0, 0, -1, 0, 1)
#define edge1 float3x3(0, 1, 0, 1, -4, 1, 0, 1, 0)
#define edge2 float3x3(-1, -1, -1, -1, 8, -1, -1, -1, -1)
#define sharpen float3x3(0, -1, 0, -1, 5, -1, 0, -1, 0)
#define box_blur float3x3(1, 1, 1, 1, 1, 1, 1, 1, 1) * 0.1111//1/9
#define gaussian_blur float3x3(1, 2, 1, 2, 4, 2, 1, 2, 1) * 0.0625//All the number in the 3x3 add up to 16, 0.0625 is 1/16.
#define emboss float3x3(-2, -1, 0, -1, 1, 1, 0, 1, 2)

cbuffer ScreenSizeBuffer : register(b0)
{
    float2 screenResolution;
    float texelSize;
    int filter;
    int2 gDist;
    float2 buffer;
    
};
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

//Grabs a pixel in a direction
float2 kpos(int index)
{
    int2 pos[9] = { int2(-1, -1), int2(0, -1), int2(1, -1), int2(-1, 0), int2(0, 0), int2(1, 0), int2(-1, 1), int2(0, 1), int2(1, 1) };
    //return int2(pos[index].x / (texelSize / screenResolution.x), pos[index].y / (texelSize / screenResolution.y)); //Doesn't really work well, can produce interesting results
    
    return pos[index] / (screenResolution.x * texelSize); //this is the same as doing int2(-texel,-texel) for the above. It's just efficient
}
//Grabs the 3x3 region of rgb pixels around uv
float3x3 region(float2 uv, int rgb)
{
    // Create each pixels for region
    float4 reg[9];//rgba of each surrounding pixel(and self)
    
    for (int i = 0; i < 9; i++)
    {
            reg[i] = shaderTexture.Sample(SampleType, uv + kpos(i));//This grabs the texture surrounding pixels
    }

    // Create 3x3 region with 3 color channels (red, green, blue)
    float3x3 rRegion;
    float3x3 gRegion;
    float3x3 bRegion;
    for (int i = 0; i < 1; i++)//Grabs the R channel of each surrounding pixel
    {
        rRegion = float3x3(
        	reg[0][i], reg[1][i], reg[2][i],
        	reg[3][i], reg[4][i], reg[5][i],
        	reg[6][i], reg[7][i], reg[8][i]
    	);
    }    
    for (int i = 1; i < 2; i++)//Grabs the G channel of each surrounding pixel
    {
        gRegion = float3x3(
        	reg[0][i], reg[1][i], reg[2][i],
        	reg[3][i], reg[4][i], reg[5][i],
        	reg[6][i], reg[7][i], reg[8][i]
    	);
    }   
    for (int i = 2; i < 3; i++)//Grabs the B Channel of each surrounding piel
    {
        bRegion = float3x3(
        	reg[0][i], reg[1][i], reg[2][i],
        	reg[3][i], reg[4][i], reg[5][i],
        	reg[6][i], reg[7][i], reg[8][i]
    	);
    }

    if (rgb == 0)
    {
        return rRegion;
    }    
    if (rgb == 1)
    {
        return gRegion;
    }   

        return bRegion;
}

//Uses a 3x3 kernal and applies it to the uv
float4 convolution(float3x3 kernal, float2 uv)
{
    float4 color;
    
    for (int i = 0; i < 3; i++)//Grabs r, g, b channels for pixels
    {
        // get region channel
        float3x3 rc = region(uv, i);
        // component wise multiplication of kernel by color channels
        float3x3 c = kernal * rc;
        
        //Adds all surrounding pixles(weighted with kernal) to the current pixel
        float3x3 r = c[0][0] + c[1][0] + c[2][0]
                + c[0][1] + c[1][1] + c[2][1]
                + c[0][2] + c[1][2] + c[2][2];
        
        color[i] = r;
    }
    color.w = 1;
    return color;

}

#define eul 2.7182818
#define pi 3.1415926

double factorial(int n)
{
    double f;
   
    for (f = 1; n > 1; n--)
        f *= n;
      
    return f;
}

double ncr(int n, int r)
{
    return factorial(n) / (factorial(n - r) * factorial(r));
}

float gaussianDistribution(float max, int index)
{
    float a = (1.0f / (sqrt(2.0f * pi * (max * max))));
    a *= eul;
    float power = (-((index * index) / (2 * (max * max))));
    float num = pow(a, power);
    return num;

}

float4 variableGaussian(float2 uv)
{
    float r;
    float g;
    float b;
    //gdist = 1, its = 3, index = 2, power = 2^index(4)
    //gdist = 2, its = 5, index = 4, power = 2^index(16)
    //gDist is our user interface to this function. It only goes up by 1's to show an increasing amount of gaussian
    //its represents the coefficients of the graph, representing how many numbers there are in the weight amount
    //index represents the index amount on the graph, this is just the amount of coefficients -1
    //divis represents the sum on the graph, this
    int its = (2 * gDist)+1;
    int index = its - 1;
    int divis = pow(2, index);
    for (int i = 0; i < its; i++)
    {
        float2 uvOff = uv; // + (float(i) / screenResolution.x);
        uvOff.x += ((float(i) - (float(its) / 2)) / (screenResolution.x * texelSize));
        int nc = ncr(index, i);
        
        float weight = (float(nc) / divis);// / 2.0f;
        r += (shaderTexture.Sample(SampleType, uvOff).r * weight);
        g += (shaderTexture.Sample(SampleType, uvOff).g * weight);
        b += (shaderTexture.Sample(SampleType, uvOff).b * weight);
        
        //uvOff = uv; // + (float(i) / screenResolution.x);
        //uvOff.y += ((float(i) - (float(its) / 2)) / (screenResolution.y * texelSize));
        //r += (shaderTexture.Sample(SampleType, uvOff).r * weight);
        //g += (shaderTexture.Sample(SampleType, uvOff).g * weight);
        //b += (shaderTexture.Sample(SampleType, uvOff).b * weight);
        
        //uvOff = uv; // + (float(i) / screenResolution.x);
        //uvOff.y += ((float(i) - (float(its) / 2)) / (screenResolution.y * texelSize));
        //uvOff.x += ((float(i) - (float(its) / 2)) / (screenResolution.x * texelSize));
        //r += (shaderTexture.Sample(SampleType, uvOff).r * weight);
        //g += (shaderTexture.Sample(SampleType, uvOff).g * weight);
        //b += (shaderTexture.Sample(SampleType, uvOff).b * weight);
        
        //uvOff = uv; // + (float(i) / screenResolution.x);
        //uvOff.y -= ((float(i) - (float(its) / 2)) / (screenResolution.y * texelSize));
        //uvOff.x -= ((float(i) - (float(its) / 2)) / (screenResolution.x * texelSize));
        //r += (shaderTexture.Sample(SampleType, uvOff).r * weight);
        //g += (shaderTexture.Sample(SampleType, uvOff).g * weight);
        //b += (shaderTexture.Sample(SampleType, uvOff).b * weight);
    }

    return float4(r, g, b, 1);

}

float4 main(InputType input) : SV_TARGET
{

    //return float4(1, 1, 1, 1);
    float texelx = texelSize / screenResolution.x;
    float texely = texelSize / screenResolution.y;
    float2 uv = input.position.xy / screenResolution;
    
    float4 col;
    switch (filter)
    {
        case 0:
    col = convolution(identity, uv);
            break;
        case 1:
    col = convolution(edge0, uv);
            break;
        case 2:
    col = convolution(edge1, uv);
            break;
        case 3:
    col = convolution(edge2, uv);
            break;
        case 4:
    col = convolution(sharpen, uv);
            break;
        case 5:
    col = convolution(box_blur, uv);
            break;
        case 6:
    col = convolution(gaussian_blur, uv);
            break;
        case 7:
    col = convolution(emboss, uv);
            break;
        case 8:
            col = variableGaussian(uv);
            break;
    }
    return col;
    
}
