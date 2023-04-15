#include "Header.hlsli"

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

struct GS_OUT
{
	float4 posH : SV_POSITION;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT SwordTrail_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float2 uv = input.posH.xy;
	float4 NoiseColor = gtxMappedTexture[2].Sample(gSamplerState, uv);
	float4 fColor = float4(10.0f, 0.0f, 0.0f, 1.0f);

	output.f4Trail = fColor;

	return output;
}