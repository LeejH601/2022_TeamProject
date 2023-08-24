#include "InputLayouts.hlsli"
#include "CurlNoise.hlsl"


struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float4 color : COLOR;
	float2 size : SCALE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint4 SpriteCoord : TEXTURECOORD; // TOTAL, CURRENT
	uint4 SpriteCoord1 : TEXTURECOORD1; // TOTAL, CURRENT
	uint4 SpriteCoord2 : TEXTURECOORD2; // TOTAL, CURRENT
	float lifetime : LIFETIME;
	float EmitTime : EMITTIME; // 방출 시작 시간 
	float emissive : EMISSIVE;
	uint rotateFlag : ROTATEFLAG;
	uint ScaleFlag : SCALEFLAG;
	uint type : TYPE;
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

	output.type = input.type;
	output.position = input.position;
	output.velocity = input.velocity;
	//output.velocity += CalculrateCulrNoise(input.position);
	output.size = input.size;
	output.alpha = 1.f;
	output.TextureIndex = input.TextureIndex;
	output.SpriteCoord.xy = int4(input.SpriteTotalCoord, SpriteAnimtaion(input, gfCurrentTime - input.EmitTime, input.lifetime, input.SpriteTotalCoord.x, input.SpriteTotalCoord.y));
	output.SpriteCoord1.xy = int4(input.SpriteTotalCoord1, SpriteAnimtaion(input, gfCurrentTime - input.EmitTime, input.lifetime, input.SpriteTotalCoord1.x, input.SpriteTotalCoord1.y));
	output.SpriteCoord2.xy = int4(input.SpriteTotalCoord2, SpriteAnimtaion(input, gfCurrentTime - input.EmitTime, input.lifetime, input.SpriteTotalCoord2.x, input.SpriteTotalCoord2.y));
	//output.SpriteCurrentCoord = ;
	output.lifetime = input.lifetime;
	output.EmitTime = input.EmitTime;
	output.emissive = input.emissive;
	output.rotateFlag = input.rotateFlag;
	output.ScaleFlag = input.ScaleFlag;
	output.type = input.type;
	output.color = input.color;
	return(output);
}