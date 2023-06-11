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

struct VS_DETAIL_OUTPUT
{
	float4 position : SV_POSITION;
	float2 size : SIZE;
	uint nInstanceID : SV_InstanceID;
};


struct GS_DETAIL_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	//uint primID : SV_PrimitiveID;
	//int textureIndex : TEXTUREINDEX;
};

#define MAX_DETAILS_INSTANCES_ONE_DRAW_CALL 4000
cbuffer cbDetailInfo : register(b7)
{
	float3 gnDetailPoisitions[MAX_DETAILS_INSTANCES_ONE_DRAW_CALL];

};


[maxvertexcount(4)]
void GS_Detail(
	point VS_DETAIL_OUTPUT input[1] : SV_POSITION,
	inout TriangleStream< GS_DETAIL_OUT > outputStream
)
{
	float3 centerW = gnDetailPoisitions[input[0].nInstanceID];
	float3 vUp = float3(0.f, 1.0f, 0.0f);
	float3 vLook = gf3CameraPosition.xyz - centerW;

	float fHalfW = input[0].size.x * 0.5f;
	float fHalfH = input[0].size.y * 0.5f;
	float4 pVertices[4];


	GS_DETAIL_OUT output;
	float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };
	vLook = normalize(vLook);
	float3 vRight = cross(float3(0.f, 1.0f, 0.0f), vLook);

	pVertices[0] = float4(centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[1] = float4(centerW + fHalfW * vRight + fHalfH * vUp, 1.0f);
	pVertices[2] = float4(centerW - fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[3] = float4(centerW - fHalfW * vRight + fHalfH * vUp, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(pVertices[i], mul(gmtxView, gmtxProjection));
		output.normalW = vLook;
		output.uv = pUVs[i];
		outputStream.Append(output);
	}
}