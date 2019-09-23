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
    //float4 lightPos = (200.0f, 50.0f, 0.0f, 1.0f);
    //float4 vLightDir = normalize(inputPixel.worldPos - lightPos);
    //float4 vReflect = reflect(vLightDir, float4(inputPixel.nrm, 0));
    //float4 vToCam = normalize(vCameraPos - inputPixel.worldPos);
    //float specDot = dot(vToCam, vReflect);
    //specDot = saturate(specDot);
    //specDot = pow(specDot, 128);
    //reflectColor += vLightColor * specDot;
    //Final Color
    finalColor = directionLightColor;// + reflectColor;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    //finalColor.a = 1.0f;
    
    return finalColor;
}