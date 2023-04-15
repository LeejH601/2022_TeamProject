
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

#define MAX_TRAILCONTROLLPOINT 50

cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints[MAX_TRAILCONTROLLPOINT];
	uint gnPoints;
}


struct GS_OUT
{
	float4 posH : SV_POSITION;
};

[maxvertexcount(4)]
void SwordTrail_GS(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream<GS_OUT> outStream
)
{
	GS_OUT output;

	float4 pVertices[4];

	pVertices[0] = gPrevPosition1;
	pVertices[1] = gNextPosition1;
	pVertices[2] = gPrevPosition2;
	pVertices[3] = gNextPosition2;

	//pVertices[0] = float4(centerW.x + fHalfW, centerW.y, centerW.z - fHalfH, 1.0f);
	//pVertices[1] = float4(centerW.x + fHalfW, centerW.y, centerW.z + fHalfH, 1.0f);
	//pVertices[2] = float4(centerW.x - fHalfW, centerW.y, centerW.z - fHalfH, 1.0f);
	//pVertices[3] = float4(centerW.x - fHalfW, centerW.y, centerW.z + fHalfH, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		output.posH = mul(pVertices[i], mul(gmtxView, gmtxProjection));

		outStream.Append(output);
	}
}