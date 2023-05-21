#include "CurlNoise.hlsl"

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

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	int type : TYPE;
	float EmitTime : EMITTIME; // ���� ���� �ð� 
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // ������ ���İ� ���(Billboard_PS)
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
	if (input.type == 0 || input.type == 5) {
		float distance = length(input.velocity);
		input.velocity += CalculrateCulrNoise(input.position).xyz;
		input.velocity = normalize(input.velocity) * distance;
	}
	
	return(input);
}
