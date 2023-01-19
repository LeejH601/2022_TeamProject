struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};



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
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

struct VS_PLANAR_SHADOW_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_PLANAR_SHADOW_OUTPUT VSPlanarShadow(VS_LIGHTING_INPUT input)
{
	VS_PLANAR_SHADOW_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);

	return(output);
}

float4 PSPlanarShadow(VS_PLANAR_SHADOW_OUTPUT input) : SV_TARGET
{
	return(float4(0.25f, 0.25f, 0.25f, 1.0f));
}
