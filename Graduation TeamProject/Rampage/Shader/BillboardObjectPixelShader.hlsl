SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t30);

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

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	uint primID : SV_PrimitiveID;
};

float4 Billboard_PS(GS_OUT input) : SV_Target
{
	float3 uvw = float3(input.uv, input.primID % 4); // ¼öÁ¤ float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture.Sample(gSamplerState, uvw);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	return(cIllumination);
}