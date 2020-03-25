
cbuffer vs_params : register( b0 )
{
	matrix trans;
	matrix view_proj;
};

cbuffer ps_params : register( b0 )
{
	float4 color;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	float4 pos = mul(float4(input.position, 1.0f), trans);
	output.pos = mul(pos, view_proj);

	float3x3 trans_rot = float3x3(trans._m00, trans._m01, trans._m02,
									trans._m10, trans._m11, trans._m12,
									trans._m20, trans._m21, trans._m22);

	output.normal = normalize(mul(input.normal, trans_rot));
    output.texCoord = input.texCoord;

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	float3 lightDir = float3(0.5f, 0.3f, 0.3f);
	lightDir = normalize(lightDir);
	float light = 0.6f + 0.6f * saturate(dot(lightDir, input.normal));

	return (color * 0.7f + diffuseMap.Sample(samLinear, input.texCoord) * 0.3f) * light;
}