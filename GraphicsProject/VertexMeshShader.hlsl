//Rule of Three
//Three things have to match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

#pragma pack_matrix(row_major)

struct InputVertex
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 nrm : NORMAL;
};

struct OutputVertex
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 nrm : NORMAL;
    float4 worldPos : WORLDPOS;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4 waveTime;
    float4 waveSpeed;
}

OutputVertex main(InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
    output.worldPos = input.pos;
	output.pos = input.pos;
    output.tex = input.tex;
	output.nrm = input.nrm;

    //float4 waves = input.pos;
    //waves.y = sin(waves.x);
    //output.pos = waves;

    float amplitude = 1.0f; //Wave height
    float frequency = 0.1f; //Distance between wave peaks
    float2 planePos = float2(output.pos.x, output.pos.z);
    float dotProduct = dot(float2(1, 1), planePos);
    output.pos.y += amplitude * sin((dotProduct * frequency) + waveTime.x);

	// Do math here (shader intrinsics)
    output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);
    output.nrm = mul(float4(output.nrm, 0), worldMatrix);
	//Don't do perspective divide, it is done automatically

	return output;
}