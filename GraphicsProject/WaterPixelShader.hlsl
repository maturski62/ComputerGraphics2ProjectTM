Texture2D Diffuse : register(t0);
TextureCube skybox : register(t1);
SamplerState samLinear : register(s0);
#include "functions.hlsli"

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
    float4 vWaterTime;
    float4 vCameraPos;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 directionLightColor = 0;
    float4 refractColor = 0;
    float4 specularColor = 0;
    
    //Directional Light
    directionLightColor += saturate(dot(-vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);

    //Reflection Color
    //float4 reflectVec = reflect(vLightDir, float4(inputPixel.nrm, 0));
    //float3 viewDirRef = normalize(inputPixel.vecRay);
    //float3 refractVec = refract(viewDirRef, inputPixel.nrm, 0.66f);
    //refractColor = skybox.Sample(samLinear, refractVec);
    //float3 viewDir2 = normalize(vCameraPos - inputPixel.worldPos);
    //float3 vRefract = reflect(viewDir2, inputPixel.nrm);
    //refractColor = skybox.Sample(samLinear, vRefract);
    //refractColor.a = 1.0f;

    //Specular
    //float3 vDirectionLight = (vLightDir.x, vLightDir.y, vLightDir.z);
    //float4 viewDir = normalize(vCameraPos - inputPixel.worldPos);
    //float4 halfVector = normalize(vLightDir + viewDir);
    //float amountOfSpec = dot(float4(inputPixel.nrm, 0), normalize(halfVector));
    //float intensity = max(pow(saturate(amountOfSpec), 4), 0);
    float specIntensity = MakeSpecular(-normalize(vLightDir), vCameraPos, inputPixel.worldPos, normalize(float4(inputPixel.nrm, 0)), SPECPOWER, 1.0f);
    specularColor = vLightColor * specIntensity;

    //Final Color
    finalColor += directionLightColor; // + refractColor;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    //finalColor.a = 1.0f;
    finalColor += specularColor;
    
    return finalColor;
}