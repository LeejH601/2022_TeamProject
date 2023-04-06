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
	//float		gfSecondsPerFirework : packoffset(c0.z); // EmmitParticles에서 사용 잠시 주석
	float		gfSpeed : packoffset(c0.z);
	int			gnFlareParticlesToEmit : packoffset(c0.w);;
	float3		gf3Gravity : packoffset(c1.x);
	int			gnMaxFlareType2Particles : packoffset(c1.w);
	float3		gfColor : packoffset(c2.x);
	int			gnParticleType : packoffset(c2.w);
	float		gfLifeTime : packoffset(c3.x);
	float		gfSize : packoffset(c3.y);
	bool		bStart : packoffset(c3.z);
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_OUT
{
	float2 sizeW : SIZE;
	uint   useBillBoard : USEBILLBOARD;
	float lifetime : LIFETIME;

};
struct VS_IN
{
	float2 sizeW : SIZE;
	uint   useBillBoard : USEBILLBOARD;
	float lifetime : LIFETIME;
};

VS_OUT Billboard_VS(VS_IN input)
{
	VS_OUT output;
	//output.centerW = input.posW;
	output.sizeW = float2(gfSize, gfSize);
	output.useBillBoard = input.useBillBoard;

	if (bStart)
		input.lifetime = gfLifeTime; // BillBoard가 아닐경우에도 사용
	
	output.lifetime = input.lifetime;
	return output;
}
