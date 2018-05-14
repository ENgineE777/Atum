cbuffer vs_params : register( b0 )
{
	float4 desc;
    matrix transform;    
};

Texture2D diffuseMap : register( t0 );
SamplerState sampler1 : register( s0 );

cbuffer ps_params : register( b0 )
{
	float4 color;    
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

PS_INPUT VS( VS_INPUT input )
{
	float4 posTemp = float4(input.position, 1.0f);	
	posTemp = mul (posTemp, transform);

	posTemp.x = -1.0f + posTemp.x/desc.x * 2.0f;
	posTemp.y = 1.0f - posTemp.y/desc.y * 2.0f;
	
	PS_INPUT output = (PS_INPUT)0;

	output.position = float4(posTemp.x, posTemp.y, desc.z, 1.0);
	output.texCoord = float2(input.texCoord.x, input.texCoord.y);

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return float4( 1.0f, 1.0f, 1.0f, diffuseMap.Sample( sampler1, input.texCoord ).x ) * color;	
}