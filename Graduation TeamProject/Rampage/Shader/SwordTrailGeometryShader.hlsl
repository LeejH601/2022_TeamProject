
cbuffer cbGameObjectInfo : register(b0)
{
	float4 gPrevPosition1: packoffset(c0);
	float4 gPrevPosition2: packoffset(c1);
	float4 gNextPosition1: packoffset(c2);
	float4 gNextPosition2: packoffset(c3);

	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
};

#define MAX_TRAILCONTROLLPOINT 100

cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints1[MAX_TRAILCONTROLLPOINT];
	float4 gControllpoints2[MAX_TRAILCONTROLLPOINT];
	uint gnPoints;
}

struct VS_OUT
{
	float4 Position : SV_POSITION;
	uint index : INDEX;
};

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
};

[maxvertexcount(4)]
void SwordTrail_GS(
	point VS_OUT input[1] : SV_POSITION,
	inout TriangleStream<GS_OUT> outStream
)
{
	GS_OUT output;

	float4 pVertices[4];

	uint index = input[0].index;

	pVertices[0] = gControllpoints1[index];
	pVertices[1] = gControllpoints1[index + 1];
	pVertices[2] = gControllpoints2[index];
	pVertices[3] = gControllpoints2[index + 1];

	float2 uvs[4];
	uvs[0] = float2(float(index) / gnPoints, 1.0f); //prev1
	uvs[1] = float2(float(index + 1) / gnPoints, 1.0f); //next1
	uvs[2] = float2(float(index) / gnPoints, 0.0f); //prev2
	uvs[3] = float2(float(index + 1) / gnPoints, 0.0f); //next2

	//pVertices[0] = gPrevPosition1;
	//pVertices[1] = gNextPosition1;
	//pVertices[2] = gPrevPosition2;
	//pVertices[3] = gNextPosition2;

	//pVertices[0] = float4(centerW.x + fHalfW, centerW.y, centerW.z - fHalfH, 1.0f);
	//pVertices[1] = float4(centerW.x + fHalfW, centerW.y, centerW.z + fHalfH, 1.0f);
	//pVertices[2] = float4(centerW.x - fHalfW, centerW.y, centerW.z - fHalfH, 1.0f);
	//pVertices[3] = float4(centerW.x - fHalfW, centerW.y, centerW.z + fHalfH, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		output.posH = mul(pVertices[i], mul(gmtxView, gmtxProjection));
		output.uv = uvs[i];

		outStream.Append(output);
	}
}