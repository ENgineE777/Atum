
cbuffer vs_params : register( b0 )
{
    float4 desc[3];
    matrix trans;
};

cbuffer ps_params : register( b0 )
{
	float4 color;
};

struct VS_INPUT
{
    float2 position : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

PS_INPUT VS( VS_INPUT input )
{
	float4 posTemp = float4(desc[0].x + desc[0].z * input.position.x,
							desc[0].y + desc[0].w * input.position.y, 0, 1.0f);
	
	posTemp = mul(posTemp, trans);

	posTemp.x = -1.0f + posTemp.x/desc[2].x * 2.0f;
	posTemp.y = 1.0f - posTemp.y/desc[2].y * 2.0f;
	
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = float4(posTemp.x, posTemp.y, posTemp.z, 1.0);
    output.texCoord = float2(desc[1].x + desc[1].z * input.position.x, 1.0f - (desc[1].y - desc[1].w * input.position.y));
	
	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
    float4 clr = diffuseMap.Sample(samLinear, input.texCoord) * color;
    if (clr.a < 0.05f)
    {
       discard;
    }

    return clr;
}