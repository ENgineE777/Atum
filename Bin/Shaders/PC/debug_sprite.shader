cbuffer vs_params : register( b0 )
{
    float4 desc[2];    
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
	float4 posTemp = float4(desc[1].x + desc[1].z * input.position.x,
						    desc[1].y + desc[1].w * input.position.y, 0, 0);

	posTemp.z = -1.0f + posTemp.x/desc[0].x * 2.0;
	posTemp.w = 1.0f - posTemp.y/desc[0].y * 2.0f;

	PS_INPUT output;

	output.position = float4( posTemp.z, posTemp.w, 0.5f, 1.0f);
	output.texCoord = input.texCoord;    

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return diffuseMap.Sample( samLinear, input.texCoord ) * color;	
}