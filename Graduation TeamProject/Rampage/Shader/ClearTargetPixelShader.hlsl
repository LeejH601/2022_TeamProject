#include "Header.hlsli"

PS_MULTIPLE_RENDER_TARGETS_OUTPUT ClearTrailTarget_PS()
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Trail = float4(0.0f, 0.0f, 0.0f, 0.05f);

	return output;
}