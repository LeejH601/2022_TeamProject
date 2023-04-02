#define MAX_FLARES 6

cbuffer cbCenterPositions : register(b8) 
{
	float2 gfCenterPositions[MAX_FLARES];
}

SamplerState gSamplerState : register(s0);

struct VS_IN
{
	float2 sizeW : SIZE;
	uint index : INDEX;
};
struct VS_OUT
{
	float2 sizeW : SIZE;
	float2 positionS : POSITION;
	uint index : INDEX;
};

VS_OUT LensFlare_VS(VS_IN input)
{
	VS_OUT output;
	//output.centerW = input.posW;
	output.sizeW = input.sizeW;
	output.positionS = gfCenterPositions[input.index].xy;
	output.index = input.index;
	return output;
}