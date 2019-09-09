
Texture2D stoneHengeDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 nrm : NORMAL;
    float4 worldPos : WORLDPOS;
};

cbuffer PSConstantBuffer : register(b1)
{
    float4 vLightDir;
    float4 vLightColor;
    float4 vPointLightPos;
    float4 vPointLightColor;
    float4 vLightRange;
    float4 vAttenuation;
    float4 vAmbient;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 pointLightColor = 0;

    float4 lightDir = vPointLightPos - inputPixel.worldPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    float amountOfLight = dot(lightDir, float4(inputPixel.nrm, 0));

    if(distance > vLightRange.x)
    {
        finalColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
        finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
        finalColor.a = 1.0f;
    }
    else
    {
        pointLightColor = amountOfLight * vPointLightColor;
        finalColor = pointLightColor * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
        finalColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
        finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
        finalColor.a = 1.0f;
    }
    
	return finalColor;
}