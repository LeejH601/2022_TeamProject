
#include "Header.hlsli"

TextureCube gtxtSkyCubeTexture : register(t30);
SamplerState gssClamp : register(s4);
SamplerState gSamplerState : register(s0);

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cColor = gtxtSkyCubeTexture.Sample(gSamplerState, input.positionL);

	output.f4Color = cColor;

	return(output);
}