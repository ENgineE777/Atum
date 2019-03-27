
cbuffer vs_params : register( b0 )
{
    float4 param;
};

struct VS_INPUT
{
	float2 position : POSITION;
	float4 color : COLOR0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	output.pos = float4(-1.0f + input.position.x/param.x * 2.0f, 1.0f - input.position.y/param.y * 2.0f, 0.0f, 1.0f);
	output.color = input.color;
	
	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
    return input.color;
}