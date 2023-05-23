#include "CurlNoise.hlsl"

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	int type : TYPE;
	float EmitTime : EMITTIME; // 방출 시작 시간 
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
	uint ParticleType : PARTICLETYPE;
};

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float4 color : COLOR;
	float2 size : SCALE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
	uint2 SpriteCurrentCoord : TEXTURECOORD1;
	uint ParticleType : PARTICLETYPE;
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

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

float interpolate(float start, float end, float fraction)
{
	return start + (end - start) * fraction;
}

uint2 SpriteAnimtaion(VS_PARTICLE_INPUT input, float AccumulatedTime, float LifeTime, uint TotalRow, uint TotalCol)
{
	float fraction = frac(AccumulatedTime / LifeTime); 
	float interval = 1.0f / (TotalRow * TotalCol);
	uint m_iCurrentCol = (int)(fraction / (interval * TotalRow));
	float remainvalue = (fraction / (interval * TotalRow)) - m_iCurrentCol;
	uint m_iCurrentRow = (int)(remainvalue * TotalRow);

	return uint2(m_iCurrentRow, m_iCurrentCol);
}

VS_PARTICLE_DRAW_OUTPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	VS_PARTICLE_DRAW_OUTPUT output = (VS_PARTICLE_DRAW_OUTPUT)0;

	output.ParticleType = input.ParticleType;
	output.position = input.position;
	output.velocity = input.velocity;
	//output.velocity += CalculrateCulrNoise(input.position);
	output.size = gfSize;

	//float fraction = (gfCurrentTime - input.EmitTime) / input.lifetime;
	//if (fraction >= 0.7) // 0.7f ~ 1.f 
	//	output.alpha = 1.f - interpolate(0.f, 1.f, (fraction - 0.7f) / 0.3f);
	//else if (fraction <= 0.2) // 0.f ~ 0.1f
	//	output.alpha = interpolate(0.f, 1.f, (fraction * 5.f));
	//else
	output.alpha = 1.f;
	output.TextureIndex = input.TextureIndex;
	output.SpriteTotalCoord = input.SpriteTotalCoord;
	output.SpriteCurrentCoord = SpriteAnimtaion(input, gfCurrentTime - input.EmitTime, input.lifetime, input.SpriteTotalCoord.x, input.SpriteTotalCoord.y);
	return(output);
}