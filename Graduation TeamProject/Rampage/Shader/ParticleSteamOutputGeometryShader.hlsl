Buffer<float4> gRandomConeBuffer : register(t32);
Buffer<float4> gRandomSphereBuffer : register(t33);

#define SPHERE_PARITLCE 0
#define SMOKE_PARITLCE 1

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
	float alpha : ALPHA;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

float rand(float2 seed)
{
	return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float RandomMinMax(float min, float max) // Min과 Max 사이의 값 반환
{
	return lerp(min, max, rand(float2(gfCurrentTime, gfElapsedTime)));
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void OutputParticleToStream(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	input.position += gf3Gravity * input.velocity * gfElapsedTime;
	//input.velocity += gf3Gravity * gfElapsedTime;
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
		OutputParticleToStream(input, output);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void SphereParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input;
	
	if (bStart) // 생성시점
	{
		float4 f4Random = float4(0.0f, 0.0f, 0.0f, 0.0f);
		particle.lifetime = gfLifeTime;
		particle.position = gmtxGameObject._41_42_43;
		particle.alpha = particle.lifetime / gfLifeTime;
		for (int i = 0; i < gnFlareParticlesToEmit; i++)
		{
			f4Random = RandomDirectionOnSphere(i + rand(gfCurrentTime));
			particle.velocity = (f4Random.xyz * gfSpeed);

			output.Append(particle);
		}
	}
	else if (input.lifetime >= 0.0f)
	{
		particle.alpha = particle.lifetime / gfLifeTime;
		OutputParticleToStream(particle, output);
	}
}




void SmokeParticles(VS_PARTICLE_INPUT input, inout PointStream<VS_PARTICLE_INPUT> output)
{
	float3 Smokevelocity =
	{
		float3(0.f, 0.f, 0.5f),
	};

	VS_PARTICLE_INPUT particle = input;
	if (bStart) // 생성시점
	{
		particle.position = gmtxGameObject._41_42_43;
		particle.alpha = particle.lifetime / gfLifeTime;
		for (int i = 0; i < 2; i++) // 2번 반복
		{
			for (int j = 0; j < 4; j++) // 같은 lifetime 4개 생성
			{
				particle.lifetime = gfLifeTime + gfLifeTime * abs(rand(gfCurrentTime + i)) * 2.f;
				Smokevelocity.x = rand(gfCurrentTime - i + j) * 2.f;
				Smokevelocity.y = -abs(rand(gfCurrentTime + i + j));
				Smokevelocity.z = rand(gfCurrentTime + j);
				Smokevelocity = -Smokevelocity;
				particle.velocity = Smokevelocity * gfSpeed;
				particle.position.x += rand(gfCurrentTime - i + j) * 8.f;
				output.Append(particle);
			}
		}
	}
	else if (particle.lifetime > 0.f)
	{
		particle.alpha = particle.lifetime / gfLifeTime;
		OutputParticleToStream(particle, output);
	}
	else
	{
		particle.position = gmtxGameObject._41_42_43;
		particle.lifetime = gfLifeTime  + gfLifeTime * abs(rand(gfCurrentTime)) * 5.f;
		particle.alpha = particle.lifetime / gfLifeTime;

		OutputParticleToStream(particle, output);
	}

}


[maxvertexcount(128)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	VS_PARTICLE_INPUT particle = input[0];
	if(gnParticleType == SPHERE_PARITLCE)
		SphereParticles(particle, output);
	else if (gnParticleType == SMOKE_PARITLCE)
		SmokeParticles(particle, output);
}