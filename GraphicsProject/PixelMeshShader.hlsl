
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
    float4 vAmbient;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 directionLightColor = 0;
    float4 pointLightColor = 0;
    float attenuation = 0;

    float4 pointLightDir = normalize(vPointLightPos - inputPixel.worldPos);
    float amountOfLight = dot(pointLightDir, float4(inputPixel.nrm, 0));
    attenuation = 1.0f - saturate(length(vPointLightPos - inputPixel.worldPos) / 10.0f);
    attenuation *= attenuation;

    //Point Light if pixel is in the range
    pointLightColor = (amountOfLight * vPointLightColor);
    pointLightColor = pointLightColor * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex) * attenuation;

    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    directionLightColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    directionLightColor.a = 1.0f;

    finalColor = directionLightColor + pointLightColor;

    // only for p light
    // for every clamp function call, you may add ambient term ex:0.45 (optional)
    // float4 finalColorforDlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), dLightColor, dlightRatio)
    // float4 finalColorforPlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), pLightColor, plightRatio);
    //  return (finalColorforPlight + finalColorforDlight + finalColorforSlight) * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
	return finalColor;
}