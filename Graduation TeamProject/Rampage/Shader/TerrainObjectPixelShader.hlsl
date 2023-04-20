#include "Header.hlsli"


cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
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
	//float		gfSecondsPerFirework : packoffset(c0.z); // EmmitParticles에서 사용 잠시 주석
	float		gfSpeed : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float		gfSize : packoffset(c3.y);
	bool		bStart : packoffset(c3.z);
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUT
{
	float2 sizeW : SIZE;
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;

};

struct VS_IN
{
	float2 sizeW : SIZE;
	float lifetime : LIFETIME;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT TerrainObject_PS(VS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	output.f4Color = cColor;

	return(output);
}
