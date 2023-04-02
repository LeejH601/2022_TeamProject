#define MAX_FLARES 6

cbuffer cbCenterPositions : register(b8)
{
	float4 gfCenterPositions[MAX_FLARES];
	float4 gxmf4UpandRight;
}

struct VS_OUT
{
	float2 sizeW : SIZE;
	float2 positionS : POSITION;
	uint index : INDEX;
};

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
	uint index : INDEX;
};

[maxvertexcount(4)]
void LensFlare_GS(point VS_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUT> outStream)
{
	float2 centerW = input[0].positionS;
	//float2 centerW = gfCenterPositions[input[0].index].xy;
	float2 vUp, vRight;
	float2 size;
	/*if (input[0].index == 2) {
		vUp = float2(gxmf4UpandRight.xy);
		vRight = float2(gxmf4UpandRight.zw);
		size = input[0].sizeW.xx;
	}
	else {*/
		vUp = float2(0.f, 1.0f);
		vRight = float2(1.0f, 0.0f);
		size = input[0].sizeW;
	//}


	float fHalfW = size.x * 0.5f;
	float fHalfH = size.y * 0.5f;
	float4 pVertices[4];

	pVertices[0] = float4(centerW + fHalfW * vRight - fHalfH * vUp, 1.0f, 1.0f);
	pVertices[1] = float4(centerW + fHalfW * vRight + fHalfH * vUp, 1.0f, 1.0f);
	pVertices[2] = float4(centerW - fHalfW * vRight - fHalfH * vUp, 1.0f, 1.0f);
	pVertices[3] = float4(centerW - fHalfW * vRight + fHalfH * vUp, 1.0f, 1.0f);

	float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };
	GS_OUT output;
	for (int i = 0; i < 4; i++)
	{
		output.posH = pVertices[i];
		output.uv = pUVs[i];
		output.index = input[0].index;
		outStream.Append(output);
	}
}