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
    float4 localPos : LOCALPOS;
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
    OutputVertex output = (OutputVertex) 0;
    output.pos = input.pos;
    output.tex = input.tex;
    output.nrm = input.nrm;

    float amplitude = 1.0f; //Wave height
    float frequency = 0.1f; //Distance between wave peaks
    float2 planePos = float2(output.pos.x, output.pos.z);
    float dotProduct = dot(float2(1, 1), planePos);
    //output.pos.y += amplitude * sin((dotProduct * frequency) + waveTime.x);

	// Do math here (shader intrinsics)
    output.localPos = output.pos;
    output.pos = mul(output.pos, worldMatrix);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projMatrix);
    output.nrm = mul(float4(output.nrm, 0), worldMatrix);
	//Don't do perspective divide, it is done automatically

    return output;
}