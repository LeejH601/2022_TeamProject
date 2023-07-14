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
	float2 fU = gmtxGameObject._12_13;// UV값 조절 
	float2 fV = gmtxGameObject._14_24;
	float3 fRGB = gmtxGameObject._21_31_44;
	float fAlpha = gmtxGameObject._23;

	input.uv.x = fU.x + (input.uv.x * (fU.y - fU.x));
	input.uv.y = fV.x + (input.uv.y * (fV.y - fV.x));
	// 스프라이트 애니메이션
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cColor = gtxtTexture[TextureIndex].Sample(gSamplerState, input.uv);
	float4 cAlphaColor = float4(1.f, 1.f, 1.f, 1.f);
	if (gmtxGameObject._34 >= 1.f) // 다른 텍스쳐 추가 사용
	{
		cAlphaColor = gtxtTexture[TextureIndex + 1].Sample(gSamplerState, input.uv);
		if (cAlphaColor.r < 1.f - gmtxGameObject._32)
			discard;
	}

	cColor.a *= fAlpha;
	cColor.rgb *= fRGB;


	output.f4Color = cColor;
	output.f4Scene = cColor;
	// 0 ~ 1
	// size, 시작 위치

	return(output);
}