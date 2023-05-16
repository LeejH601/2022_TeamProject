Buffer<float4> gRandomSphereBuffer : register(t50);

#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2
#define TRAIL_PARTILCE 3
#define ATTACK_PARTICLE 4

//#define TYPE_DEFAULT -1
#define TYPE_EMITTER 0
#define TYPE_SIMULATOR 1
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
	float EmitTime : EMITTIME; // 방출 시작 시간 
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4); // ParticleIndex로 사용 (_11)
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

float rand(float2 seed) // 양수 
{
	return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float RandomMinMax(float min, float max) // Min과 Max 사이의 값 반환
{
	return lerp(min, max, rand(float2(gfCurrentTime, gfElapsedTime)));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
VS_PARTICLE_INPUT OutputParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;
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
			particle.SpriteTotalCoord = iTextureCoord;
			particle.TextureIndex = iTextureIndex;
			particle.type = TYPE_SIMULATOR;
			float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);
			particle.position = gmtxGameObject._41_42_43;
			//particle.alpha = 1.f;
			particle.EmitTime = gfCurrentTime; // 방출 시간 기록
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
			particle.SpriteTotalCoord = iTextureCoord;
			particle.TextureIndex = iTextureIndex;
			particle.type = TYPE_SIMULATOR;
			particle.position = gmtxGameObject._41_42_43;
			//particle.alpha = particle.lifetime / gfLifeTime;
			particle.EmitTime = gfCurrentTime; // 방출 시간 기록
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

void AttackParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;

	if (particle.type == TYPE_EMITTER)
	{
		output.Append(particle);
		if (bEmit)
		{
			particle.SpriteTotalCoord = iTextureCoord;
			particle.TextureIndex = iTextureIndex;
			particle.type = TYPE_SIMULATOR;
			particle.position = gmtxGameObject._41_42_43;
			particle.EmitTime = gfCurrentTime; // 방출 시간 기록
			particle.lifetime = gfLifeTime;
			particle.velocity = float3(0.f, 0.f, 0.f);
			output.Append(particle);
		}
	}
	else if (particle.type == TYPE_SIMULATOR)
	{
		if ((gfCurrentTime - particle.EmitTime) <= particle.lifetime)
		{
			output.Append(input);
		}
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
			particle.SpriteTotalCoord = iTextureCoord;
			particle.TextureIndex = iTextureIndex;
			particle.type = TYPE_SIMULATOR;
			particle.EmitTime = gfCurrentTime; // 방출 시간 기록
			float3 position = gmtxGameObject._41_42_43;
			for (int i = 0; i < 20; i++) // 2번 반복
			{
				particle.lifetime = gfLifeTime;
				//particle.alpha = 1.f;
				particle.velocity = float3(0.f, 3.f + rand(gfCurrentTime + i) * 3.f, 0.f);
				particle.position.y = position.y;
				particle.position.x = position.x + (i * 0.2f - (20 / 2) * 0.2f) * 0.5f + rand(gfCurrentTime + i) * 2.f;
				particle.position.z = position.z + (i * 0.2f - (20 / 2) * 0.2f) * 0.5f + rand(gfCurrentTime - i) * 2.f;
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

[maxvertexcount(30)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input[0];
	if (gnParticleType == SPHERE_PARTILCE)
		SphereParticles(particle, output);
	else if (gnParticleType == RECOVERY_PARTILCE)
		RecoveryParticles(particle, output);
	else if (gnParticleType == SMOKE_PARTILCE)
		SmokeParticles(particle, output);
	else if (gnParticleType == ATTACK_PARTICLE)
		AttackParticles(particle, output);

}
