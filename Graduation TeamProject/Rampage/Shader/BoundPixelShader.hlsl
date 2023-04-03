#include "Header.hlsli"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Bound(VS_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = float4(1.0f, 0.0f, 0.0f, 1.0f);
	output.f4Color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return(output);
}