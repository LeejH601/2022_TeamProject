
#define FRAME_BUFFER_WIDTH 1920
#define FRAME_BUFFER_HEIGHT 1080

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
	matrix m_xmf4x4OrthoProjection : packoffset(c8);
	matrix gmtxInverseProjection : packoffset(c12);
	matrix gmtxInverseView : packoffset(c16);
	float3 gf3CameraPosition : packoffset(c20);
	//float3 gf3CameraDirection : packoffset(c17);
};

SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float2 size : SIZE;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSUIObject(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;
	matrix Matrix = gmtxInverseView;
	float2 uvPosition = gmtxGameObject._41_42; // 0 ~ 1
	Matrix._11_22 = gmtxGameObject._11_22;
	Matrix._33 = 0.f;
	output.position = mul(mul(mul(float4(input.position, 1.0f), Matrix), gmtxInverseView), m_xmf4x4OrthoProjection);
	output.position += float4(uvPosition, 0.f, 0.f);

	output.uv = input.uv;

	return(output);
}
