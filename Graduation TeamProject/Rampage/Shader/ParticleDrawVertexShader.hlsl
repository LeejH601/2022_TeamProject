struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	int type : TYPE;
	int  bTerrain : TERRAIN;
};

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float4 color : COLOR;
	float2 size : SCALE;
	float alpha : ALPHA;
	int  bTerrain : TERRAIN;
};

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

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

VS_PARTICLE_DRAW_OUTPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	VS_PARTICLE_DRAW_OUTPUT output = (VS_PARTICLE_DRAW_OUTPUT)0;

	output.position = input.position;
	output.velocity = input.velocity;
	output.size = gfSize;
	output.alpha = 1.f;
	output.bTerrain = input.bTerrain;
	return(output);
}


//VertexOutput VertexShaderMain(VertexInput input, Particle particle)
//{
//	VertexOutput output;
//
//	// Get particle's direction vector
//	float3 direction = normalize(particle.velocity);
//
//	// Calculate the particle's right and up vectors
//	float3 right = cross(float3(0, 1, 0), direction);
//	float3 up = cross(direction, right);
//
//	// Create the rotation matrix using the particle's direction, right, and up vectors
//	float4x4 rotationMatrix;
//	rotationMatrix[0] = float4(right, 0);
//	rotationMatrix[1] = float4(up, 0);
//	rotationMatrix[2] = float4(direction, 0);
//	rotationMatrix[3] = float4(0, 0, 0, 1);
//
//	// Transform the particle's position by the rotation matrix and view-projection matrix
//	float4 transformedPosition = mul(viewProjectionMatrix, mul(rotationMatrix, float4(particle.position, 1)));
//
//	// Output the transformed position and texture coordinate
//	output.position = transformedPosition;
//	output.texcoord = float2(0, 0);
//	return output;
//}