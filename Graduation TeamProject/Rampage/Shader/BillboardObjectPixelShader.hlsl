#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t30);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // ������ ���İ� ���(Billboard_PS)
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT Billboard_PS(GS_OUT input) : SV_Target1
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float3 uvw = float3(input.uv, input.primID % 4); // ���� float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture.Sample(gSamplerState, uvw);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	//cColor.a *= gnTexturesMask * 0.01f; // 0~100���� �޾� 0.00 ~1.00���� ����
	float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	output.f4Scene = cIllumination;
	output.f4Color = cIllumination;

	return(output);
}