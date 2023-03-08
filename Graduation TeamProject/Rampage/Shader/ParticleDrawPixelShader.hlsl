#include "Header.hlsli"

Texture2D<float4> gtxtParticleTexture : register(t31);

SamplerState gSamplerState : register(s0);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfSecondsPerFirework : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float		gfSize : packoffset(c3.y);
};


struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	uint type : PARTICLETYPE;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);
	cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	cColor *= float4(gfColor, 1.f);

	output.f4Scene = cColor;
	output.f4Color = cColor;

	return(output);
}
