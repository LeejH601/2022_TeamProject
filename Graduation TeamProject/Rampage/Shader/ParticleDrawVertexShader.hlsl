struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	uint type : PARTICLETYPE;
};

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float size : SCALE;
	uint type : PARTICLETYPE;
};

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfSecondsPerFirework : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float		gfSize : packoffset(c3.y);
};

#define PARTICLE_TYPE_EMITTER		0
#define PARTICLE_TYPE_SHELL			1
#define PARTICLE_TYPE_FLARE01		2
#define PARTICLE_TYPE_FLARE02		3
#define PARTICLE_TYPE_FLARE03		4

#define SHELL_PARTICLE_LIFETIME		3.0f
#define FLARE01_PARTICLE_LIFETIME	2.5f
#define FLARE02_PARTICLE_LIFETIME	1.5f
#define FLARE03_PARTICLE_LIFETIME	2.0f

VS_PARTICLE_DRAW_OUTPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	VS_PARTICLE_DRAW_OUTPUT output = (VS_PARTICLE_DRAW_OUTPUT)0;

	output.position = input.position;
	output.size = gfSize;
	output.type = input.type;

	if (gnParticleType == PARTICLE_TYPE_EMITTER) 
	{ 
		output.color = float4(1.0f, 0.1f, 0.1f, 1.0f);
	}
	else if (gnParticleType == PARTICLE_TYPE_SHELL) 
	{ 
		output.color = float4(0.1f, 0.0f, 1.0f, 1.0f);
	}
	else if (gnParticleType == PARTICLE_TYPE_FLARE01) 
	{ 
		output.color = float4(1.0f, 1.0f, 0.1f, 1.0f); 
		output.color *= (input.lifetime / gfLifeTime); 
	}
	else if (gnParticleType == PARTICLE_TYPE_FLARE02) 
		output.color = float4(1.0f, 0.1f, 1.0f, 1.0f);
	else if (gnParticleType == PARTICLE_TYPE_FLARE03) 
	{ 
		output.color = float4(1.0f, 0.1f, 1.0f, 1.0f); 
		output.color *= (input.lifetime / gfLifeTime); 
	}

	return(output);
}