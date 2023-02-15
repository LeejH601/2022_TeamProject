cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}


Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUT
{
	//float3 centerW : POSITION;
	float2 sizeW : SIZE;
};
struct VS_IN
{
	//float3 posW : POSITION;
	float2 sizeW : SIZE;
};

VS_OUT Billboard_VS(VS_IN input)
{
	VS_OUT output;
	//output.centerW = input.posW;
	output.sizeW = input.sizeW;
	return output;
}
