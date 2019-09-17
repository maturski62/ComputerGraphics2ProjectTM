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
    float4 vPointLightPos;
    float4 vPointLightColor;
    float4 vAmbient;
    float4 vSpotLightPos;
    float4 vSpotLightDir;
    float4 vSpotLightColor;
    float4 vSpotLightConeRatio;
    float4 drawingSkybox;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 directionLightColor = 0;

    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    directionLightColor.a = 1.0f;

    finalColor = directionLightColor;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    
    return finalColor;
}