
#include "Header.hlsli"

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
};

#include "Light.hlsl"

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