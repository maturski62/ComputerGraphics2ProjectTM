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
    float4 vCameraPos;
}

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 finalColor = 0;
    float4 directionLightColor = 0;
    float4 reflectColor = 0;
    
    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);

    //Reflection Color
    float4 reflectVec = reflect(vLightDir, float4(inputPixel.nrm, 0));
    //Specular
    float4 viewDir = normalize(vCameraPos - inputPixel.worldPos);
    float4 halfVector = normalize((vLightDir) + viewDir);
    float intensity = max(pow(saturate(dot(float4(inputPixel.nrm, 0), normalize(halfVector))), 4), 0);
    float specularColor = vLightColor * 5.0f * intensity;
    //Final Color
    finalColor = directionLightColor + reflectColor + specularColor;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    //finalColor.a = 1.0f;
    
    return finalColor;
}