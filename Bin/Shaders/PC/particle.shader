
cbuffer vs_params : register( b0 )
{
	matrix trans;
	matrix view_proj;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color  : COLOR0;
};

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	float4 pos = mul(float4(input.position, 1.0f), trans);
	output.pos = mul(pos, view_proj);

    output.texCoord = input.texCoord;
    output.color = input.color;

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return diffuseMap.Sample(samLinear, input.texCoord) * input.color;
}