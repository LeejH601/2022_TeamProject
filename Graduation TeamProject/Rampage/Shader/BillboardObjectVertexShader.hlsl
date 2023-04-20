cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfSpeed : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float2		gfSize : packoffset(c3.y);
	bool		bEmit : packoffset(c3.w);
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUT
{
	float2 sizeW : SIZE;
	float lifetime : LIFETIME;
	uint  useBillBoard : USEBILLBOARD; // BILLBOARD ��� ����
};

struct VS_IN
{
	float2 sizeW : SIZE;
	float lifetime : LIFETIME;
	uint  useBillBoard : USEBILLBOARD; // BILLBOARD ��� ����
};

VS_OUT Billboard_VS(VS_IN input)
{
	VS_OUT output;
	//output.centerW = input.posW;
	output.sizeW = gfSize;
	output.useBillBoard = input.useBillBoard;
	input.lifetime = gfLifeTime; // BillBoard�� �ƴҰ�쿡�� ���
	
	output.lifetime = input.lifetime;
	return output;
}
