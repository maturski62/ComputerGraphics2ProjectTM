Texture2D Diffuse : register(t0);
TextureCube skybox : register(t1);
SamplerState samLinear : register(s0);

struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 nrm : NORMAL;
    float4 worldPos : WORLDPOS;
    float4 localPos : LOCALPOS;
};

cbuffer PSConstantBuffer : register(b1)
{
    float4 vLightDir;
    float4 vLightColor;
    float4 vWaterTime;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 directionLightColor = 0;
    
    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    //Final Color
    finalColor = directionLightColor;
    //inputPixel.tex.x += vWaterTime.x / 100.0f;
    //inputPixel.tex.y += vWaterTime.x / 100.0f;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    finalColor.a = 1.0f;
    
    return finalColor;
}