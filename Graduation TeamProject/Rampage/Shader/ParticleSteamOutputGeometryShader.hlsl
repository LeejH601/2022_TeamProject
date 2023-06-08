#include "CurlNoise.hlsl"
#include "InputLayouts.hlsli"

Buffer<float4> gRandomSphereBuffer : register(t50);

#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2
#define TRAIL_PARTILCE 3
#define ATTACK_PARTICLE 4
#define VERTEXPOINT_PARTICLE 5
#define SLASHHIT_PARTICLE 6
#define IMPACT_PARTICLE 7
#define TERRAIN_PARTICLE 8

//#define TYPE_DEFAULT -1

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
	matrix gmtxTexture : packoffset(c4); // ParticleIndex?†Ïèô???†Ïèô?ôÂç†?(_11)
	uint gnTexturesMask : packoffset(c8); // ?†Ïèô?ôÂç†?ôÏòô?†Ïèô???†Ïèô?ôÂç†?ùÍ≥§???†Ïèô?ôÂç†?Billboard_PS)
};

float rand(float2 seed) // ?†Ïèô?ôÂç†?
{
	return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float RandomMinMax(float min, float max) // Min?†Ïèô??Max ?†Ïèô?ôÂç†?ôÏòô?†Ïèô???†Ïèô???†Ïèô?ôÌôò
{
	return lerp(min, max, rand(float2(gfCurrentTime, gfElapsedTime)));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
VS_PARTICLE_INPUT OutputParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	//input.velocity += CalculrateCulrNoise(input.position).xyz;
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;
	//input.velocity += float3(0.0f, -9.8f*20.0f, 0.0f) * gfElapsedTime;
	//input.velocity += CalculrateCulrNoise(input.position).xyz;
	return input;
}

void OutputRandomParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;

	output.Append(input);
}
void OutputRandomAccelerationParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	input.velocity += gf3Gravity * gfElapsedTime;

	output.Append(input);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
float4 RandomDirectionOnSphere(float fOffset)
{
	int u = uint(gfCurrentTime + fOffset /*+ frac(gfCurrentTime) * 1000.0f*/) % 256;
	return(normalize(gRandomSphereBuffer.Load(u)));
}

void OutputEmberParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	if ((gfCurrentTime - input.EmitTime) <= input.lifetime)
	{
		output.Append(OutputParticleToStream(input, output));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void SphereParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	float lifedTime = gfCurrentTime - particle.EmitTime;
	if (lifedTime <= particle.lifetime)
	{
		particle = OutputParticleToStream(particle, output);
		//float3 resistanceDir = gmtxGameObject._41_42_43 - particle.position;
		//float distance = length(resistanceDir) - (100.0f/2);
		//distance = max(0.0f, distance);
		//resistanceDir = normalize(resistanceDir) * length(particle.velocity);
		//float resistanceValue = distance / 100.0f; // ?†Ïåç?êÏòô ?†Ïå•Í≥§Ïòô 20.0f
		//particle.velocity += resistanceDir * resistanceValue; // ?†Ïèô?ôÂç†?àÎì∏???†Ïèô?ôÂç†?ôÏòô???†Ïã≠?∏Ïòô?†Ïèô??
		/*float distance = length(particle.velocity);
		particle.velocity += CalculrateCulrNoise(particle.position).xyz;
		particle.velocity = normalize(particle.velocity) * distance;*/
		output.Append(particle);
	}
}


void SmokeParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	float3 Smokevelocity = { -float3(rand(gfCurrentTime), 0.f, 0.5f) };
	VS_PARTICLE_INPUT particle = input;

	if (particle.lifetime > 0.f)
		OutputRandomParticleToStream(particle, output);

}

void AttackParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if ((gfCurrentTime - particle.EmitTime) <= particle.lifetime)
	{
		output.Append(input);
	}
}

void RecoveryParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if ((gfCurrentTime - particle.EmitTime) <= particle.lifetime)
	{
		float3 position = gmtxGameObject._41_42_43;
		float MoveValue = particle.position.y - position.y;
		if (MoveValue > 7.f)
		{

			particle.position.y = position.y;
		}
		//else if (MoveValue > 3.5f)
		//{
		//	//particle.alpha = particle.lifetime / gfLifeTime;
		//}
		OutputRandomAccelerationParticleToStream(particle, output);
	}

	//}
	//else
	//{
	//	output.Append(particle);
	//}
}

void SlashHitParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output) {
	VS_PARTICLE_INPUT particle = input;

	float lifedTime = gfCurrentTime - particle.EmitTime;
	if (lifedTime <= particle.lifetime)
	{
		output.Append(particle);
	}
}

void ImpactParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output) {
	VS_PARTICLE_INPUT particle = input;

	float lifedTime = gfCurrentTime - particle.EmitTime;
	if (lifedTime <= particle.lifetime)
	{
		output.Append(particle);
	}
}

void TerrainParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if ((gfCurrentTime - particle.EmitTime) <= particle.lifetime)
	{
		output.Append(input);
	}
}

[maxvertexcount(16)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input[0];
	if (particle.type == SPHERE_PARTILCE)
		SphereParticles(particle, output);
	else if (particle.type == RECOVERY_PARTILCE)
		RecoveryParticles(particle, output);
	else if (particle.type == SMOKE_PARTILCE)
		SmokeParticles(particle, output);
	else if (particle.type == ATTACK_PARTICLE)
		AttackParticles(particle, output);
	else if (particle.type == VERTEXPOINT_PARTICLE)
		SphereParticles(particle, output);
	else if(particle.type == SLASHHIT_PARTICLE)
		SlashHitParticles(particle, output);
	else if (particle.type == IMPACT_PARTICLE) {
		ImpactParticles(particle, output);
	}
	else if (particle.type == TERRAIN_PARTICLE)
		TerrainParticles(particle, output);
}
