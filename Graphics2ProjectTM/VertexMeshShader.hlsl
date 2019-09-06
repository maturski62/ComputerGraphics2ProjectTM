//Rule of Three
//Three things have to match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

#pragma pack_matrix(row_major)

struct InputVertex
{
	float3 xyz : POSITION;
	float3 uvw : TEXCOORD;
	float3 nrm : NORMAL;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float4 rgba : OCOLOR;
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
	output.xyzw = float4(input.xyz, 1);
	output.rgba.rgb = input.nrm;
	// Do math here (shader intrinsics)
	output.xyzw = mul(output.xyzw, worldMatrix);
	output.xyzw = mul(output.xyzw, viewMatrix);
	output.xyzw = mul(output.xyzw, projMatrix);
	//Don't do perspective divide, it is done automatically

	return output;
}

//VIDEO 3 46:17