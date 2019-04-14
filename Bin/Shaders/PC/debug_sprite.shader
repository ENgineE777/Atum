cbuffer vs_params : register( b0 )
{
    float4 desc[3];    
};

cbuffer ps_params : register( b0 )
{
	float4 color;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord    : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : COLOR;
};

Texture2D diffuseMap : register( t0 );
SamplerState samLinear : register( s0 );

PS_INPUT VS( VS_INPUT input )
{
	float4 posTemp = float4(desc[2].x + desc[1].z * input.position.x,
	                        desc[2].y + desc[1].w * input.position.y,
	                        cos(desc[2].z), sin(desc[2].z));

	float2 pos = float2(desc[1].x + posTemp.x * posTemp.z - posTemp.y * posTemp.w,
						desc[1].y + posTemp.y * posTemp.z + posTemp.x * posTemp.w);

	pos.x = -1.0f + pos.x/desc[0].x * 2.0;
	pos.y =  1.0f - pos.y/desc[0].y * 2.0f;

	PS_INPUT output;

	output.position = float4(pos.x, pos.y, 0.5f, 1.0f);
	output.texCoord = input.texCoord;    

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return diffuseMap.Sample( samLinear, input.texCoord ) * color;	
}