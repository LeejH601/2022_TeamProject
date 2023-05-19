#include "Header.hlsli"
#include "CurlNoise.hlsl"

Buffer<float4> gRandomSphereBuffer : register(t50);
Texture2D gtxtTexture[29] : register(t51);

SamplerState gSamplerState : register(s0);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4); // ParticleIndex로 사용 (_11)
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfLifeTime : packoffset(c0.z);
	bool		bEmit : packoffset(c0.w);

	uint2		iTextureCoord : packoffset(c1.x);
	uint		iTextureIndex : packoffset(c1.z);
	uint		gnParticleType : packoffset(c1.w);

	float3		gf3Gravity : packoffset(c2.x);
	float		gfSpeed : packoffset(c2.w);

	float3		gfColor : packoffset(c3.x);
	uint		gnFlareParticlesToEmit : packoffset(c3.w);

	float2		gfSize : packoffset(c4.x);

};

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
	uint ParticleType : PARTICLETYPE;
};

float4 PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET1  // SphereTexture t50을 제외
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	//float4 cColor = gtxtTexture[0].Sample(gSamplerState, input.uv); // 2번이
	float4 cColor = gtxtTexture[(uint)(input.TextureIndex - 1)].Sample(gSamplerState, input.uv); // 2번이
	cColor *= float4(gfColor, 1.f);
	//cColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
	cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	cColor.a *= input.alpha; // 사라지는 효과 구현 위해(위 아래로 내려가는 파티클을 위해)

	return(cColor);
}
