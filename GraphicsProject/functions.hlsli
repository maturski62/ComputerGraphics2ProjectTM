#define SPECPOWER 128

float MakeSpecular(float4 vToLight, float4 camPos, float4 pixelPos, 
                    float4 pixelNrm, float specPower, float specIntensity)
{
    //Vector to the camera
    float3 vToCam = normalize(camPos.xyz - pixelPos.xyz);
    //Reflection vector froom light and surface normal
    float3 vReflect = normalize(reflect(-vToLight.xyz, pixelNrm.xyz));
    //Find angle between vToCam and vReflect
    float specDot = saturate(dot(vToCam.xyz, vReflect.xyz));
    //Raise dot to certain power
    float intensity = pow(specDot, specPower);
    //Light ratio of the surface
    float lightRatio = saturate(dot(vToLight, pixelNrm));
    //return spec intensity
    return saturate(intensity * specIntensity * lightRatio);
}