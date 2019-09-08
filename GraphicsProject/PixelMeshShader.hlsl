
Texture2D stoneHengeDiffuse : register(t0);
SamplerState samLinear : register(s0);



struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 nrm : NORMAL;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    //float4 color = float4(inputPixel.nrm, 1); // Color based on the normal
	//float4 color = {1.0f, 0.0f, 0.0f, 1.0f};
    //return color;

    float4 finalColor = 0;
    float4 vLightDir = {-0.577f, 0.577f, -0.577f, 1.0f};
    float4 vLightColor = { 0.5f, 0.5f, 0.5f, 1.0f };

    finalColor += saturate(dot(vLightDir, float4(inputPixel.nrm, 0)) * vLightColor);
    finalColor *= stoneHengeDiffuse.Sample(samLinear, inputPixel.tex);
    finalColor.a = 1.0f;
	return finalColor;
}