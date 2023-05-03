Buffer<float4> gRandomConeBuffer : register(t32);
Buffer<float4> gRandomSphereBuffer : register(t33);

#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2
#define TRAIL_PARTILCE 3

//#define TYPE_DEFAULT -1
#define TYPE_EMITTER 0
#define TYPE_SIMULATOR 1
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


struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	int type : TYPE;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

float rand(float2 seed) // ��� 
{
	return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float rand_(float2 seed) // -1.f ~ 1.f
{
	return (rand(seed) - 0.5f) * 2.f;
}

float RandomMinMax(float min, float max) // Min�� Max ������ �� ��ȯ
{
	return lerp(min, max, rand(float2(gfCurrentTime, gfElapsedTime)));
}


/////////////////////////////////////////////////////////////////////////////////////////////////
VS_PARTICLE_INPUT OutputParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;
	input.velocity += float3(0.0f, -9.8f*60, 0.0f) * gfElapsedTime;
	input.lifetime -= gfElapsedTime;
	return input;
}

void OutputRandomParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;
	input.lifetime -= gfElapsedTime;

	output.Append(input);
}
void OutputRandomAccelerationParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	input.velocity += gf3Gravity * gfElapsedTime;
	input.lifetime -= gfElapsedTime;

	output.Append(input);
}
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
		output.Append(OutputParticleToStream(input, output));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void SphereParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if (particle.type == TYPE_EMITTER)
	{
		output.Append(particle);
		if (bEmit)
		{
			particle.type = TYPE_SIMULATOR;
			float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);
			particle.position = gmtxGameObject._41_42_43;
			//particle.alpha = 1.f;
			for (int i = 0; i < gnFlareParticlesToEmit; i++)
			{
				particle.lifetime = gfLifeTime;
				f4Random = RandomDirectionOnSphere(i + rand(gfCurrentTime));
				particle.velocity = (f4Random.xyz * gfSpeed);
				output.Append(particle);
			}
		}
	}
	else if (particle.type == TYPE_SIMULATOR)
	{
		if (particle.lifetime > 0.f)
		{
			particle = OutputParticleToStream(particle, output);
			output.Append(particle);
		}
	}
	
}


void SmokeParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	float3 Smokevelocity = { -float3(rand(gfCurrentTime), 0.f, 0.5f) };
	VS_PARTICLE_INPUT particle = input;

	if (particle.type == TYPE_EMITTER)
	{
		output.Append(particle);
		if (bEmit)
		{
			particle.type = TYPE_SIMULATOR;
			particle.position = gmtxGameObject._41_42_43;
			//particle.alpha = particle.lifetime / gfLifeTime;

			particle.lifetime = gfLifeTime;
			particle.velocity = Smokevelocity * gfSpeed;
			output.Append(particle);
		}
		

	}
	else if (particle.type == TYPE_SIMULATOR)
	{
		if (particle.lifetime > 0.f)
			OutputRandomParticleToStream(particle, output);
	}

}

void RecoveryParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if (particle.type == TYPE_EMITTER)
	{
		output.Append(particle);
		if (bEmit)
		{
			particle.type = TYPE_SIMULATOR;
			float3 position = gmtxGameObject._41_42_43;
			for (int i = 0; i < 20; i++) // 2�� �ݺ�
			{
				particle.lifetime = gfLifeTime;
				//particle.alpha = 1.f;
				particle.velocity = float3(0.f, 3.f + rand(gfCurrentTime + i) * 3.f, 0.f);
				particle.position.y = position.y;
				particle.position.x = position.x + (i * 0.2f - (20 / 2) * 0.2f) * 0.5f + rand_(gfCurrentTime + i) * 2.f;
				particle.position.z = position.z + (i * 0.2f - (20 / 2) * 0.2f) * 0.5f + rand_(gfCurrentTime - i) * 2.f;
				output.Append(particle);
			}
		}
	}
	else if (particle.type == TYPE_SIMULATOR)
	{
		if (particle.lifetime > 0.f)
		{
			particle.type = TYPE_SIMULATOR;
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

	}
	else
	{
		output.Append(particle);
	}
}

//void TrailParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
//{
//	VS_PARTICLE_INPUT particle = input;
//	if (bStart) // ��������
//	{
//		float3 position = gmtxGameObject._41_42_43;
//		//float3 rotation = gmtxGameObject._11_12_13;
//		particle.lifetime = gfLifeTime;
//		//particle.alpha = 1.f;
//		//particle.velocity = rotation;
//	
//		particle.position = position;
//		output.Append(particle);
//	}
//	else if (particle.lifetime > 0.f)
//	{
//		particle.lifetime -= gfElapsedTime;
//		//particle.alpha  -= gfElapsedTime * 1.5f;
//		//if (particle.alpha < 0.f)
//		//	particle.alpha = 0.f;
//		output.Append(particle);
//	}
//
//}

[maxvertexcount(128)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input[0];
	if (gnParticleType == SPHERE_PARTILCE)
		SphereParticles(particle, output);
	else if (gnParticleType == RECOVERY_PARTILCE)
		RecoveryParticles(particle, output);
	else if (gnParticleType == SMOKE_PARTILCE)
		SmokeParticles(particle, output);
	//else if (gnParticleType == TRAIL_PARTILCE)
	//	TrailParticles(particle, output);
}
