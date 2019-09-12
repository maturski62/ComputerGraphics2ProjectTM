
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
    float4 vSpotLightPos;
    float4 vSpotLightDir;
    float4 vSpotLightColor;
    float4 vSpotLightConeRatio;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    //return vSpotLightDir;

    float4 finalColor = 0;
    float4 directionLightColor = 0;
    float4 pointLightColor = 0;
    float pointLightRange = 10.0f;
    float4 spotLightColor = 0;
    float spotLightRange = 10.0f;
    float attenuation = 0;

    //Point Light
    float4 pointLightDir = normalize(vPointLightPos - inputPixel.worldPos);
    float amountOfPointLight = dot(pointLightDir, float4(inputPixel.nrm, 0));
    attenuation = 1.0f - saturate(length(vPointLightPos - inputPixel.worldPos) / pointLightRange);
    attenuation *= attenuation;
    pointLightColor = (amountOfPointLight * vPointLightColor) * attenuation;

    //Spot Light
    float4 spotLightDir = normalize(vSpotLightPos - inputPixel.worldPos);
    float surfaceRatio = dot(-spotLightDir, vSpotLightDir);
    float amountOfSpotLight = dot(spotLightDir, float4(inputPixel.nrm, 0));
    float spotFactor = (surfaceRatio > vSpotLightConeRatio.x) ? 1.0f : 0.0f;
    spotLightColor = (spotFactor * amountOfSpotLight * vSpotLightColor);

    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    directionLightColor.a = 1.0f;

    finalColor = directionLightColor + pointLightColor + spotLightColor;
    finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);

    // only for p light
    // for every clamp function call, you may add ambient term ex:0.45 (optional)
    // float4 finalColorforDlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), dLightColor, dlightRatio)
    // float4 finalColorforPlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), pLightColor, plightRatio);
    //  return (finalColorforPlight + finalColorforDlight + finalColorforSlight) * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
	return finalColor;
}