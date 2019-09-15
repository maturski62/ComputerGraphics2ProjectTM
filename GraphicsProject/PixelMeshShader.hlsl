
Texture2D Diffuse : register(t0);
//TextureCube skybox : register(t1);
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
    //return vSpotLightDir;;
    //float3 localPixelPos;
    //localPixelPos.x = inputPixel.localPos.x;
    //localPixelPos.y = inputPixel.localPos.y;
    //localPixelPos.z = inputPixel.localPos.z;
    //return skybox.Sample(samLinear, localPixelPos);
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
    float surfaceRatio = dot(-spotLightDir, vSpotLightDir); //Need for attenuation
    float amountOfSpotLight = dot(spotLightDir, float4(inputPixel.nrm, 0)); //How much light will the pixel recieve from the spot light
    float innerAngle = vSpotLightConeRatio.x; //I set this to 0.8f, the closer to 1.0f, the smaller the cone.
    float outerAngle = vSpotLightConeRatio.x - 0.2f; //Subtracted 0.2 to get a bigger radius because the closer to 0.0f, the bigger the cone.
    attenuation = 1.0f - saturate(length(vSpotLightPos - inputPixel.worldPos) / spotLightRange); //Basic Attenuation just like the point light. Range is 10.0f for me.
    attenuation *= 1.0f - saturate((innerAngle - surfaceRatio) / (innerAngle - outerAngle)); //Falloff between inner and outer angle
    attenuation *= attenuation; //Making falloff not as linear to get a more realistic look.
    spotLightColor = (amountOfSpotLight * vSpotLightColor) * attenuation; //Final spot light color

    //Directional Light
    directionLightColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    directionLightColor.a = 1.0f;

    finalColor = directionLightColor + pointLightColor + spotLightColor;
    finalColor *= Diffuse.Sample(samLinear, inputPixel.tex);
    
	return finalColor;
}