SamplerState gSamplerState : register(s0);

#define MAX_LIGHTS 4

struct CB_TOOBJECTSPACE
{
	matrix		mtxToTexture;
	float4		f4Position;
};

struct VS_DETAIL_INPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
	float3 color : COLOR;
};

struct VS_DETAIL_OUTPUT
{
	float4 position : SV_POSITION;
	float2 size : SIZE;
	float3 color : COLOR;
	float4 uvs[MAX_LIGHTS] : TEXCOORD1;
};

cbuffer cbToLightSpace : register(b6)
{
	CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
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
	output.color = input.color;

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gcbToLightSpaces[i].f4Position.w != 0.0f) output.uvs[i] = mul(output.position, gcbToLightSpaces[i].mtxToTexture);
	}

	return output;
}