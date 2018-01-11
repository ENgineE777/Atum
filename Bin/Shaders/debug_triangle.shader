
cbuffer vs_params : register( b0 )
{
	matrix trans;
	matrix view_proj;
};

cbuffer ps_params : register(b0)
{
	float4 color;
	float3 lightDir;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : TEXCOORD0;
	float4 color : COLOR0;
};

struct PS_INPUT
{
	float4 pos    : SV_POSITION;
	float3 normal : TEXCOORD0;
	float4 color  : COLOR0;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	float4 pos = mul(float4(input.position, 1.0f), trans);
	output.pos = mul(pos, view_proj);

	float3x3 trans_rot = float3x3(trans._m00, trans._m10, trans._m20,
								  trans._m01, trans._m11, trans._m21,
								  trans._m02, trans._m12, trans._m22);

	output.normal = normalize(mul(input.normal, trans_rot));
	output.color = input.color;

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	float3 lDir = normalize(lightDir);
	float light = 0.5f + 0.5f * saturate(dot(lDir, input.normal));
	return color * light * input.color;
}