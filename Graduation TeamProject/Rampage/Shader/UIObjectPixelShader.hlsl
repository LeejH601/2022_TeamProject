#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Buffer<float4> gRandomSphereBuffer : register(t50);
Texture2D gtxtTexture[29] : register(t51);
SamplerState gClampState : register(s4);

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
	float2 size : SIZE;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSUIObject(VS_TEXTURED_OUTPUT input)
{
	uint TextureIndex = gmtxGameObject._33; // TextureIndex
	float2 fUV = gmtxGameObject._12_13;// UV값 조절
	float3 fRGB = gmtxGameObject._21_31_44;
	float fAlpha = gmtxGameObject._23;

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cColor = gtxtTexture[TextureIndex].Sample(gSamplerState, input.uv);
	cColor.rgb *= fRGB;
	cColor.a *= fAlpha;
	output.f4Color = cColor;
	output.f4Scene = cColor;
	if (fUV.x < input.uv.x || fUV.y < input.uv.y)
		discard;
	return(output);
}