
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
    float4 finalDirectionLightColor = 0;
    float4 finalPointLightColor = 0;
    float4 pointLightColor = 0;
    float attenuation = 0;
    
    float4 pointLightDir = vPointLightPos - inputPixel.worldPos;
    float distance = length(pointLightDir);
    pointLightDir = normalize(pointLightDir);
    float amountOfLight = dot(pointLightDir, float4(inputPixel.nrm, 0));
    //attenuation = 1.0f - saturate(length(vPointLightPos - inputPixel.pos) / vLightRange.x);
    //attenuation *= attenuation;
    
    if(distance < vLightRange.x)
    {
        //Point Light if pixel is in the range
        pointLightColor = (amountOfLight * vPointLightColor);
        finalColor = pointLightColor * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    }

    //float4 pointLightDir = normalize(vPointLightPos - inputPixel.worldPos);
    //float lightRatio = saturate(dot(pointLightDir, float4(inputPixel.nrm, 0)));
    //attenuation = 1.0f - saturate(length(vPointLightPos - inputPixel.pos) / 10.0f);
    //attenuation *= attenuation;
    //finalPointLightColor = (lightRatio * vPointLightColor * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex));// * attenuation;
    //
    ////Directional Light no matter the circumstances
    //finalDirectionLightColor = saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    //finalDirectionLightColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    //finalDirectionLightColor.a = 1.0f;
    //finalColor = (finalDirectionLightColor + finalPointLightColor);

    finalColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    finalColor.a = 1.0f;


    // only for p light
    // for every clamp function call, you may add ambient term ex:0.45 (optional)
    // float4 finalColorforDlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), dLightColor, dlightRatio)
    // float4 finalColorforPlight = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), pLightColor, plightRatio);
    //  return (finalColorforPlight + finalColorforDlight + finalColorforSlight) * stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
	return finalColor;
}