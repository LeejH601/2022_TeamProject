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
	float2 sizeW : SIZE;
	bool   useBillBoard : USEBILLBOARD;
};
struct VS_IN
{
	float2 sizeW : SIZE;
	uint   useBillBoard : USEBILLBOARD;
};

VS_OUT Billboard_VS(VS_IN input)
{
	VS_OUT output;
	//output.centerW = input.posW;
	output.sizeW = input.sizeW;
	output.useBillBoard = input.useBillBoard;
	return output;
}
