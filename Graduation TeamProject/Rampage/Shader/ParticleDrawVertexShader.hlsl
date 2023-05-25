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
	uint2 SpriteTotalCoord : TEXTURECOORD;
	uint2 SpriteCurrentCoord : TEXTURECOORD1;
	float lifetime : LIFETIME;
	float EmitTime : EMITTIME; // 방출 시작 시간 
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

	output.position = input.position;
	output.velocity = input.velocity;
	//output.velocity += CalculrateCulrNoise(input.position);
	output.size = gfSize;
	output.alpha = 1.f;
	output.TextureIndex = input.TextureIndex;
	output.SpriteTotalCoord = input.SpriteTotalCoord;
	output.SpriteCurrentCoord = SpriteAnimtaion(input, gfCurrentTime - input.EmitTime, input.lifetime, input.SpriteTotalCoord.x, input.SpriteTotalCoord.y);
	output.lifetime = input.lifetime;
	output.EmitTime = input.EmitTime;
	return(output);
}


//if (fTimeElapsed >= 0.0f)
//{
//	m_fAccumulatedTime += fTimeElapsed;
//
//	float fraction = std::fmodf(m_fAccumulatedTime / m_fLifeTime, 1.0f);
//	interval = 1.0f / (m_iTotalRow * m_iTotalCol);
//
//	m_iCurrentCol = (int)(fraction / (interval * m_iTotalRow));
//	float remainvalue = (fraction / (interval * m_iTotalRow)) - m_iCurrentCol;
//	m_iCurrentRow = (int)(remainvalue * m_iTotalRow);
//
//	if (m_fAccumulatedTime > m_fLifeTime)
//	{
//		m_iCurrentRow = 0;
//		m_iCurrentCol = 0;
//		m_fAccumulatedTime = 0.0f;
//		m_bAnimation = false;
//	}
//}
//m_xmf4x4World._11 = 1.0f / float(m_iTotalRow);
//m_xmf4x4World._22 = 1.0f / float(m_iTotalCol);
//m_xmf4x4World._31 = float(m_iCurrentRow) / float(m_iTotalRow);
//m_xmf4x4World._32 = float(m_iCurrentCol) / float(m_iTotalCol);