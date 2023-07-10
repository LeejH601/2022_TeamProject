#include "InputLayouts.hlsli"
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



cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // ������ ���İ� ���(Billboard_PS)
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
	uint type = input.type;
	if (type == 0 || type == 3 || type == 5 || type == 9 || type == 10) {
		float distance = length(input.velocity);
		input.velocity += CalculrateCulrNoise(input.position).xyz;
		//input.velocity += CalculrateCulrNoise(input.position).xyz * (1.0f - float(input.rotateFlag));
		input.velocity = normalize(input.velocity) * distance;
	}
	
	return(input);
}
