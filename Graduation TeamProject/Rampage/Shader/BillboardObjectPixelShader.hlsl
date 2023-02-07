SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t24);

//struct VS_TEXTURED_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD;
//};

//float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//
//	return(cColor);
//}

//DXGI_FORMAT Object.Sample(
//	SamplerState S,
//	float Location
//	[, int Offset]
//);
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
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
	float4 cIllumination = Lighting(input.posW, input.normalW);
	float3 uvw = float3(input.uv, input.primID % 4); // ¼öÁ¤ float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture.Sample(gSamplerState, uvw);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	cColor = lerp(cColor, cIllumination, 0.2f);
	cColor.a = cTexture.a;
	return(cColor);
}