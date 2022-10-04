Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 PS_Tmp(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}