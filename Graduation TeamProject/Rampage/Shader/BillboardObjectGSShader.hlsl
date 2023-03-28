cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
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

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);


struct VS_OUT
{
	float2 sizeW : SIZE;
	uint   useBillBoard : USEBILLBOARD; // BILLBOARD 사용 유무
};

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void GS(point VS_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUT> outStream)
{
	float3 centerW = gmtxGameObject._41_42_43;
	float3 vUp = float3(0.f, 1.0f, 0.0f);
	float3 vLook = gf3CameraPosition.xyz - centerW;
	if (!input[0].useBillBoard)
	{
		vLook = float3(0.f, 0.f, -1.0f);
		vUp = float3(0.f, 0.f, 1.f);
	}
	vLook = normalize(vLook);
	float3 vRight = cross(float3(0.f, 1.0f, 0.0f), vLook);

	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;
	float4 pVertices[4];
	pVertices[0] = float4(centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[1] = float4(centerW + fHalfW * vRight + fHalfH * vUp, 1.0f);
	pVertices[2] = float4(centerW - fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[3] = float4(centerW - fHalfW * vRight + fHalfH * vUp, 1.0f);

	float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };
	GS_OUT output;
	for (int i = 0; i < 4; i++)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(pVertices[i], mul(gmtxView, gmtxProjection));
		output.normalW = vLook;
		output.uv = mul(float3(pUVs[i], 1.0f), (float3x3)(gmtxGameObject)).xy;
		output.primID = primID;
		outStream.Append(output);
	}
}