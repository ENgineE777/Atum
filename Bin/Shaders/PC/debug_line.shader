
cbuffer vs_params : register( b0 )
{
	matrix view_proj;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color    : COLOR;
};

struct PS_INPUT
{
	float4 pos   : SV_POSITION;
	float4 color : COLOR;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	output.pos = mul(float4(input.position, 1.0f), view_proj);
    output.color = input.color;

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return input.color;
}