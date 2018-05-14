
cbuffer vs_params : register( b0 )
{
	matrix world;
	matrix trans;
	matrix sh_trans;
};

cbuffer ps_params : register( b0 )
{
	float4 color;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 texCoord : TEXCOORD1;
	float4 lightViewPosition : TEXCOORD2;
};

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

Texture2D shdMap : register(t1);
SamplerState shdLinear : register(s1);


PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	float4 pos = float4(input.position, 1.0f);
	output.Pos = mul(pos, trans);
	output.texCoord = input.texCoord;

	output.lightViewPosition = mul(pos, world);
	output.lightViewPosition = mul(output.lightViewPosition, sh_trans);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 projTex = input.lightViewPosition;
	projTex.xyz /= projTex.w;

	projTex.xy = projTex.xy *0.5f + 0.5f;
	projTex.y = 1 - projTex.y;

	projTex.xy = saturate(projTex.xy);
	projTex.z = clamp(projTex.z, 0.0f, 0.99f);

	float4 shadowMapDepth = shdMap.Sample(shdLinear, projTex.xy);

	float shd = 0.5;

	if (projTex.z < shadowMapDepth.r)
	{
		shd = 1.0f;
	}

	float4 clr = diffuseMap.Sample(samLinear, input.texCoord) * color * shd;
	clr.a = 1.0f;

	return clr;
}