#include "InputLayouts.hlsli"

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
	float lifetime : LIFETIME;
	float EmitTime : EMITTIME; // 방출 시작 시간 
	float emissive : EMISSIVE;
	uint rotateFlag : ROTATEFLAG;
	uint ScaleFlag : SCALEFLAG;
	uint type : TYPE;
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
static float3 gf3PositionsTerrain[4] = { float3(-1.0f, 0.0f, +1.0f), float3(+1.0f, 0.0f, +1.0f), float3(-1.0f, 0.0f, -1.0f), float3(+1.0f, 0.0f, -1.0f) };
//static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };
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

	output.alpha = input[0].alpha;
	output.color = input[0].color;
	output.TextureIndex = input[0].TextureIndex;
	output.lifetime = input[0].lifetime;
	output.EmitTime = input[0].EmitTime;
	output.emissive = input[0].emissive;


	float3 WorldNormal = float3(0, 1.0f, 0);
	float3 velocityInCameraSpace = mul(float4(input[0].velocity, 0.0f), gmtxView);
	float3 velocity = normalize(velocityInCameraSpace);

	velocityInCameraSpace = normalize(float3(velocityInCameraSpace.x, velocityInCameraSpace.y, 0.0f));
	float3 normalVelocity = float3(-velocityInCameraSpace.y, velocityInCameraSpace.x, 0.0f);

	float3 Positions[4] = { velocityInCameraSpace - normalVelocity, velocityInCameraSpace + normalVelocity, 
		-velocityInCameraSpace - normalVelocity, -velocityInCameraSpace + normalVelocity };

	float cosCeta = dot(WorldNormal, velocityInCameraSpace);

	if (cosCeta == 0.00001f) cosCeta = 0.1f;
	float ceta = acos(cosCeta); // radian
	if (velocityInCameraSpace.x < 0)
		ceta = -ceta;

	uint rotateFlag = input[0].rotateFlag;
	if (input[0].rotateFlag == 0)
		ceta = 0;

	float3x3 rotate = float3x3(cos(ceta), -sin(ceta), 0,
		sin(ceta), cos(ceta), 0,
		0, 0, 1);

	uint scaleFlag = input[0].ScaleFlag;
	float scaleValue = 1.0 - min(velocity.z, float(input[0].ScaleFlag));
	if (input[0].ScaleFlag == 0)
		scaleValue = 1.0f;

	float sizeX = input[0].size.x;
	float sizeY = input[0].size.y;

	if (input[0].type == 6) {
		float lifedTime = gfCurrentTime - input[0].EmitTime;
		float value = lifedTime / input[0].lifetime;
		sizeX *= 1.0f - value;
	}
	

	for (int i = 0; i < 4; i++)
	{
		gf3Positions[i].x = gf3Positions[i].x * sizeX * 0.5f;
		gf3Positions[i].y = gf3Positions[i].y * (sizeY * scaleValue) * 0.5f;
		gf3Positions[i] = mul(gf3Positions[i], rotate);

		float3 positionW = input[0].position;

		output.position = mul(float4(positionW, 1.0f), gmtxView);
		output.position.xyz += gf3Positions[i];
		output.position = mul(output.position, gmtxProjection);

	/*	float3 positionW = mul((gf3Positions[i]), (float3x3)(gmtxInverseView)) + input[0].position;
		
		output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);*/
		output.uv = mul(float3(gf2QuadUVs[i], 1.0f), (float3x3)(xmf4x4Coord)).xy;
		outputStream.Append(output);
	}
	outputStream.RestartStrip();
}


//m_xmf4x4World._11 = 1.0f / float(m_iTotalRow);
//m_xmf4x4World._22 = 1.0f / float(m_iTotalCol);
//m_xmf4x4World._31 = float(m_iCurrentRow) / float(m_iTotalRow);
//m_xmf4x4World._32 = float(m_iCurrentCol) / float(m_iTotalCol);