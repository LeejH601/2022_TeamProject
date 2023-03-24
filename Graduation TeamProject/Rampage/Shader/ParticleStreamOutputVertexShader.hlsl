struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	float spantime : SPANTIME;
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
	return(input);
}