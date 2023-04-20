#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t30);
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
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
};

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfSpeed : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float2		gfSize : packoffset(c3.y);
	bool		bEmit : packoffset(c3.w);
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

PS_MULTIPLE_RENDER_TARGETS_OUTPUT Billboard_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float3 uvw = float3(input.uv, input.primID % 4); // 수정 float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture.Sample(gClampState, uvw);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	cColor.rgb *= gfColor;
	cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	output.f4Scene = cIllumination;
	output.f4Color = cIllumination;

	return(output);
}