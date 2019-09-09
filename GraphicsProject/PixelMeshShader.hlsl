
Texture2D stoneHengeDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 nrm : NORMAL;
};

cbuffer PSConstantBuffer : register(b1)
{
    float4 vLightDir;
    float4 vLightColor;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    finalColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    finalColor.a = 1.0f;
	return finalColor;
}