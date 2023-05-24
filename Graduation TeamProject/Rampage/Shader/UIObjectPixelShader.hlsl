#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Buffer<float4> gRandomSphereBuffer : register(t50);
Texture2D gtxtTexture[29] : register(t51);
SamplerState gClampState : register(s4);

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
	matrix m_xmf4x4OrthoProjection : packoffset(c8);
	matrix gmtxInverseProjection : packoffset(c12);
	matrix gmtxInverseView : packoffset(c16);
	float3 gf3CameraPosition : packoffset(c20);
	//float3 gf3CameraDirection : packoffset(c17);
};

//struct VS_TEXTURED_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD;
//	uint TextureIndex :TEXTUREINDEX;
//};

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSUIObject(VS_TEXTURED_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cColor = gtxtTexture[3].Sample(gSamplerState, input.uv);
	output.f4Color = cColor;
	output.f4Scene = cColor;
	return(output);
}