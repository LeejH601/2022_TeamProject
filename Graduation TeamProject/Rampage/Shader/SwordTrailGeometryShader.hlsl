
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
	matrix m_xmf4x4OrthoProjection : packoffset(c8);
	matrix gmtxInverseProjection : packoffset(c12);
	matrix gmtxInverseView : packoffset(c16);
	float3 gf3CameraPosition : packoffset(c20);
	//float3 gf3CameraDirection : packoffset(c17);
};

#define MAX_TRAILCONTROLLPOINT 200
#define MAX_COLORCURVES 8
cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints1[MAX_TRAILCONTROLLPOINT];
	float4 gControllpoints2[MAX_TRAILCONTROLLPOINT];
	float4 gfR_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfG_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfB_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfColorCurveTimes[MAX_COLORCURVES / 4];
	uint gnCurves;
	float gfNoiseConstants;
	uint gnPoints;
	float gnOffsetTime;
	float gnEmissiveFactor;
	uint gnBaseTextureIndex;
	uint gnNoiseTextureIndex;
	float gnScale;
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

[maxvertexcount(8)]
void SwordTrail_GS(
	point VS_OUT input[1] : SV_POSITION,
	inout TriangleStream<GS_OUT> outStream
)
{
	GS_OUT output;

	float4 pVertices[4];

	uint index = input[0].index;
	float4 directionToFar1 = gControllpoints2[index] - gControllpoints1[index];
	float4 directionToFar2 = gControllpoints2[index + 1] - gControllpoints1[index + 1];

	float4 FarPoints[2];
	FarPoints[0] = gControllpoints1[index];
	FarPoints[0].xyz += (directionToFar1 * gnScale);

	FarPoints[1] = gControllpoints1[index + 1];
	FarPoints[1].xyz += (directionToFar2 * gnScale);

	/*float4 dir[2];
	dir[0] = gControllpoints2[index] - gControllpoints1[index];
	dir[1] = gControllpoints1[index + 1] - gControllpoints1[index];
	float lengthScale = length(dir[0]);

	float3 crossvec = cross(normalize(dir[0]).xyz, normalize(dir[1]).xyz) * lengthScale;*/

	pVertices[0] = gControllpoints1[index];
	pVertices[1] = gControllpoints1[index + 1];
	/*pVertices[0] = gControllpoints1[index] + float4(crossvec, 0.0f);
	pVertices[1] = gControllpoints1[index + 1] + float4(crossvec, 0.0f);*/

	pVertices[2] = FarPoints[0];
	pVertices[3] = FarPoints[1];

	//pVertices[4] = gControllpoints1[index] - float4(crossvec, 0.0f);
	//pVertices[5] = gControllpoints1[index + 1] - float4(crossvec, 0.0f);


	float2 uvs[4];
	uvs[0] = saturate(float2(float(index) / float(gnPoints), 1.0f)); //prev1
	uvs[1] = saturate(float2(float(index + 1) / float(gnPoints), 1.0f)); //next1

	uvs[2] = saturate(float2(float(index) / float(gnPoints), 0.0f)); //prev2
	uvs[3] = saturate(float2(float(index + 1) / float(gnPoints), 0.0f)); //next2

	//uvs[4] = saturate(float2(float(index) / float(gnPoints), 1.0f)); //prev1
	//uvs[5] = saturate(float2(float(index + 1) / float(gnPoints), 1.0f)); //next1


	outStream.RestartStrip();
	for (int i = 0; i < 4; i++)
	{
		output.posH = mul(pVertices[i], mul(gmtxView, gmtxProjection));
		output.uv = uvs[i];

		outStream.Append(output);
	}
}