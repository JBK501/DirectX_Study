cbuffer MatrixBuffer
{
	matrix worldTransform;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// 모든 버텍스에 대해서 수행
VOut main( float4 pos : POSITION, float2 tex : TEXCOORD0 )
{
	VOut output;

	output.position = mul(pos, worldTransform);
	output.tex = tex;

	return output;
}