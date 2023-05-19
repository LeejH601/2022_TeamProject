#define SPHERE_PARTILCE 0
#define RECOVERY_PARTILCE 1
#define SMOKE_PARTILCE 2
#define TRAIL_PARTILCE 3
#define ATTACK_PARTICLE 4
#define TERRAIN_PARTICLE 5

struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float4 color : COLOR;
	float2 size : SCALE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint2 TextureCoord : TEXTURECOORD;
	uint2 CurrentCoord : TEXTURECOORD1;
	uint ParticleType : PARTICLETYPE;
};

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint2 TextureCoord : TEXTURECOORD;
	uint ParticleType : PARTICLETYPE;
};


cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
};

static float3 gf3Positions[4] = { float3(-1.0f, +1.0f, 0.0f), float3(+1.0f, +1.0f, 0.0f), float3(-1.0f, -1.0f, 0.0f), float3(+1.0f, -1.0f, 0.0f) };
static float3 gf3TerrainPositions[4] = { float3(-1.0f, 0.0f, 1.0f), float3(+1.0f, 0.0f, +1.0f), float3(-1.0f, 0.0f, -1.0f), float3(+1.0f,  0.0f, -1.0f) };
static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };
[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_DRAW_OUTPUT input[1], inout TriangleStream<GS_PARTICLE_DRAW_OUTPUT> outputStream)
{
	GS_PARTICLE_DRAW_OUTPUT output = (GS_PARTICLE_DRAW_OUTPUT)0;

	matrix xmf4x4Coord = gmtxGameObject;



	xmf4x4Coord._11 = 1.0f / float(input[0].TextureCoord.x);
	xmf4x4Coord._22 = 1.0f / float(input[0].TextureCoord.y);
	xmf4x4Coord._31 = float(input[0].CurrentCoord.x) / float(input[0].TextureCoord.x);
	xmf4x4Coord._32 = float(input[0].CurrentCoord.y) / float(input[0].TextureCoord.y);

	output.ParticleType = input[0].ParticleType;
	output.alpha = input[0].alpha;
	output.color = input[0].color;
	output.TextureIndex = input[0].TextureIndex;

	float3 positionW;
	for (int i = 0; i < 4; i++)
	{
		if (TERRAIN_PARTICLE == output.ParticleType)
		{
			gf3TerrainPositions[i].x = gf3TerrainPositions[i].x * input[0].size.x * 0.5f;
			gf3TerrainPositions[i].z = gf3TerrainPositions[i].z * input[0].size.y * 0.5f;
			positionW = gf3TerrainPositions[i] + input[0].position;
		}
		else
		{
			gf3Positions[i].x = gf3Positions[i].x * input[0].size.x * 0.5f;
			gf3Positions[i].y = gf3Positions[i].y * input[0].size.y * 0.5f;
			positionW = mul((gf3Positions[i]), (float3x3)(gmtxInverseView)) + input[0].position;
		}
		output.position = mul(mul(float4((positionW), 1.0f), gmtxView), gmtxProjection);
		output.uv = mul(float3(gf2QuadUVs[i], 1.0f), (float3x3)(xmf4x4Coord)).xy;
		outputStream.Append(output);
	}

	outputStream.RestartStrip();

}

