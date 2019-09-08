
//Texture2D txDiffuse : register(t0);
//SamplerState samLinear : register(s0);

struct OutputVertex
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nrm : NORMAL;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 color = float4(inputPixel.nrm , 1);
	//float4 color = {1.0f, 0.0f, 0.0f, 1.0f};
	return color;
	//return float4(inputPixel.uvw, 1);

	//float4 finalColor = 0;
	//
	//finalColor *= txDiffuse.Sample(samLinear, inputPixel.uvw);
	//finalColor.a = 1;
	//return finalColor;
}