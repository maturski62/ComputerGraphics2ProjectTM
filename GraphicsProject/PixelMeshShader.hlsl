
Texture2D stoneHengeDiffuse : register(t0);
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

    //Center Point Light
    float4 pointLightDir = normalize(vPointLightPos - inputPixel.worldPos);
    float amountOfPointLight = dot(pointLightDir, float4(inputPixel.nrm, 0));
    attenuation = 1.0f - saturate(length(vPointLightPos - inputPixel.worldPos) / pointLightRange);
    attenuation *= attenuation;
    pointLightColor = (amountOfPointLight * vPointLightColor) * attenuation;

    //Spot Light
    float4 spotLightDir = normalize(vSpotLightPos - inputPixel.worldPos);
    float surfaceRatio = dot(-spotLightDir, vSpotLightDir);
    float amountOfSpotLight = dot(spotLightDir, float4(inputPixel.nrm, 0));
    float innerAngle = vSpotLightConeRatio.x;
    float outerAngle = vSpotLightConeRatio.x - 0.2f;
    attenuation = 1.0f - saturate(length(vSpotLightPos - inputPixel.worldPos) / spotLightRange);
    attenuation *= 1.0f - saturate((innerAngle - surfaceRatio) / (innerAngle - outerAngle));
    //attenuation *= attenuation;
    spotLightColor = (amountOfSpotLight * vSpotLightColor) * attenuation;

    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    directionLightColor.a = 1.0f;

    finalColor = directionLightColor + pointLightColor + spotLightColor;
    finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    
	return finalColor;
}