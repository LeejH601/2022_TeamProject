SamplerState gSamplerState : register(s0);

struct VS_DETAIL_INPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
};

struct VS_DETAIL_OUTPUT
{
	float4 position : SV_POSITION;
	float2 size : SIZE;
};

#define MAX_DETAILS_INSTANCES_ONE_DRAW_CALL 4000
cbuffer cbDetailInfo : register(b7)
{
	float4 gnSizeAndOffset;
	float3 gnDetailPoisitions[MAX_DETAILS_INSTANCES_ONE_DRAW_CALL];
};

VS_DETAIL_OUTPUT VS_Detail(VS_DETAIL_INPUT input)
{
	VS_DETAIL_OUTPUT output;
	output.position = float4(input.position, 1.0f);
	output.size = input.size;

	return output;
}