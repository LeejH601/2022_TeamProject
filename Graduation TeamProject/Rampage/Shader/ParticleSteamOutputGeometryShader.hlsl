#define PARTICLE_TYPE_EMITTER		0
#define PARTICLE_TYPE_SHELL			1
#define PARTICLE_TYPE_FLARE01		2
#define PARTICLE_TYPE_FLARE02		3
#define PARTICLE_TYPE_FLARE03		4

#define SHELL_PARTICLE_LIFETIME		3.0f
#define FLARE01_PARTICLE_LIFETIME	2.5f
#define FLARE02_PARTICLE_LIFETIME	1.5f
#define FLARE03_PARTICLE_LIFETIME	2.0f

Buffer<float4> gRandomConeBuffer : register(t32);
Buffer<float4> gRandomSphereBuffer : register(t33);

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

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	uint type : PARTICLETYPE;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//float4 RandomDirection(float fOffset)
//{
//	int u = uint(gfCurrentTime + fOffset + frac(gfCurrentTime) * 1000.0f) % 1024;
//	return(normalize(gRandomBuffer.Load(u)));
//}

//void EmmitParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
//{
//	float4 f4Random = RandomDirection(input.type);
//	if (input.lifetime <= 0.0f)
//	{
//		VS_PARTICLE_INPUT particle = input;
//
//		particle.type = PARTICLE_TYPE_SHELL;
//		particle.position = input.position + (input.velocity * gfElapsedTime * f4Random.xyz);
//		particle.velocity = input.velocity + (f4Random.xyz * 16.0f);
//		particle.lifetime = gfLifeTime + (f4Random.y * 0.5f);
//
//		output.Append(particle);
//
//		//input.lifetime = gfSecondsPerFirework * 0.2f + (f4Random.x * 0.4f);
//	}
//	else
//	{
//		input.lifetime -= gfElapsedTime;
//	}
//
//	output.Append(input);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
void OutputParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += input.velocity * gfElapsedTime;
	input.velocity += gf3Gravity * gfElapsedTime;
	input.lifetime -= gfElapsedTime;

	output.Append(input);
}
//
//void ShellParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
//{
//	if (input.lifetime <= 0.0f)
//	{
//		VS_PARTICLE_INPUT particle = input;
//		float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);
//
//		particle.type = PARTICLE_TYPE_FLARE01;
//		particle.position = input.position + (input.velocity * gfElapsedTime * 2.0f);
//		particle.lifetime = gfLifeTime;
//
//		for (int i = 0; i < gnFlareParticlesToEmit; i++)
//		{
//			f4Random = RandomDirection(input.type + i);
//			particle.velocity = input.velocity + (f4Random.xyz * 18.0f);
//
//			output.Append(particle);
//		}
//
//		particle.type = PARTICLE_TYPE_FLARE02;
//		particle.position = input.position + (input.velocity * gfElapsedTime);
//		for (int j = 0; j < abs(f4Random.x) * gnMaxFlareType2Particles; j++)
//		{
//			f4Random = RandomDirection(input.type + j);
//			particle.velocity = input.velocity + (f4Random.xyz * 10.0f);
//			particle.lifetime = gfLifeTime + (f4Random.x * 0.4f);
//
//			output.Append(particle);
//		}
//	}
//	else
//	{
//		OutputParticleToStream(input, output);
//	}
//}
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
float4 RandomDirectionOnSphere(float fOffset)
{
	int u = uint(gfCurrentTime + fOffset /*+ frac(gfCurrentTime) * 1000.0f*/) % 256;
	return(normalize(gRandomSphereBuffer.Load(u)));
}

float4 RandomDirectionOnCone(float fOffset)
{
	int u = uint(gfCurrentTime + fOffset + frac(gfCurrentTime) * 1000.0f) % 256;
	return(normalize(gRandomSphereBuffer.Load(u)));
}

void OutputEmberParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	if (input.lifetime > 0.0f)
	{
		OutputParticleToStream(input, output);
	}
}

void GenerateEmberParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{

	if (input.lifetime <= 0.0f)
	{
		// gfLifeTime
		VS_PARTICLE_INPUT particle = input;

		particle.type = PARTICLE_TYPE_FLARE03;
		//particle.position = input.position + (input.velocity * gfElapsedTime);
		particle.lifetime = gfLifeTime;
		for (int i = 0; i < 64; i++)
		{
			float4 f4Random = RandomDirectionOnSphere(i);
			particle.velocity = input.velocity + (f4Random.xyz * 25.0f);


			output.Append(particle);
		}
	}
	else
	{
		OutputParticleToStream(input, output);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void SphereParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{

	if ((gfLifeTime <= 0.0f)) // 생성시점
	{
		VS_PARTICLE_INPUT particle = input;
		float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);

		particle.type = PARTICLE_TYPE_FLARE01;
		particle.position = input.position + (input.velocity * gfElapsedTime * 2.0f);


		for (int i = 0; i < gnFlareParticlesToEmit; i++)
		{
			f4Random = RandomDirectionOnSphere(i);
			particle.velocity = input.velocity + (f4Random.xyz * gfSpeed);

			output.Append(particle);
		}
	}
	else
	{
		OutputParticleToStream(input, output);
	}
}

void ConeParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{

	if ((gfLifeTime <= 0.0f)) // 생성시점
	{
		VS_PARTICLE_INPUT particle = input;
		float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);

		particle.type = PARTICLE_TYPE_FLARE01;
		particle.position = input.position + (input.velocity * gfElapsedTime * 2.0f);


		for (int i = 0; i < gnFlareParticlesToEmit; i++)
		{
			f4Random = RandomDirectionOnSphere(i);
			particle.velocity = input.velocity + (f4Random.xyz * gfSpeed);

			output.Append(particle);
		}
	}
	else
	{
		OutputParticleToStream(input, output);
	}
}

[maxvertexcount(128)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input[0];
	ConeParticles(particle, output);
	//if (particle.type == PARTICLE_TYPE_EMITTER) 
	//	EmmitParticles(particle, output);
	//else if (particle.type == PARTICLE_TYPE_SHELL) 
	//	ShellParticles(particle, output);
	//else if ((particle.type == PARTICLE_TYPE_FLARE01) || (particle.type == PARTICLE_TYPE_FLARE03)) 
	//	OutputEmberParticles(particle, output);
	//else if (particle.type == PARTICLE_TYPE_FLARE02) 
	//	GenerateEmberParticles(particle, output);
}