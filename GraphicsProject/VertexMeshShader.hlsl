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
};

cbuffer SHADER_VARIABLES : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

OutputVertex main(InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
	output.pos = input.pos;
    output.tex = input.tex;
	output.nrm = input.nrm;
	// Do math here (shader intrinsics)
    output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projMatrix);
    output.nrm = mul(float4(output.nrm, 0), worldMatrix);
	//Don't do perspective divide, it is done automatically

	return output;
}