#include "Header.hlsli"

Texture2D gtxtTexture[29] : register(t51);

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

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	float alpha : ALPHA;
};

float4 PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET1
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cColor = gtxtTexture[2].Sample(gSamplerState, input.uv);
	cColor *= float4(gfColor, 1.f);
	cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	cColor.a *= input.alpha; // 사라지는 효과 구현 위해(위 아래로 내려가는 파티클을 위해)
	return(cColor);
}
