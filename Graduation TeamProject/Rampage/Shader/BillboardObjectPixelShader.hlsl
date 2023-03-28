SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t30);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
}

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
	float3 uvw = float3(input.uv, input.primID % 4); // 수정 float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture.Sample(gSamplerState, uvw);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	//cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	return(cIllumination);
}