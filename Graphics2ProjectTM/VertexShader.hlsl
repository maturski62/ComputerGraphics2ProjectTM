//Rule of Three
//Three things have to match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

struct InputVertex
{
	float4 xyzw : POSITION;
	float4 rgba : COLOR;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
};

OutputVertex main( InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
}

//VIDEO 3 46:17