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
	float		gfLifeTime : packoffset(c0.z);
	bool		bEmit : packoffset(c0.w);

	uint2		iTextureCoord : packoffset(c1.x);
	uint		iTextureIndex : packoffset(c1.z);
	uint		gnParticleType : packoffset(c1.w);

	float3		gf3Gravity : packoffset(c2.x);
	float		gfSpeed : packoffset(c2.w);

	float3		gfColor : packoffset(c3.x);
	uint		gnFlareParticlesToEmit : packoffset(c3.w);

	float2		gfSize : packoffset(c4.x);

};

SamplerState gSamplerState : register(s0);

struct VS_OUT
{
	float2 sizeW : SIZE;
	float lifetime : LIFETIME;
	uint  useBillBoard : USEBILLBOARD; // BILLBOARD 사용 유무
	//int textureIndex : TEXTUREINDEX;
};

struct VS_IN
{
	float2 sizeW : SIZE;
	float lifetime : LIFETIME;
	uint  useBillBoard : USEBILLBOARD; // BILLBOARD 사용 유무
	//int textureIndex : TEXTUREINDEX;
};

VS_OUT Billboard_VS(VS_IN input)
{
	VS_OUT output;
	output.sizeW = gfSize;
	output.useBillBoard = input.useBillBoard;
	input.lifetime = gfLifeTime; // BillBoard가 아닐경우에도 사용
	//output.textureIndex = (uint)(gmtxTexture._11); // 0~100까지 쓰니까 
	output.lifetime = input.lifetime;
	return output;
}
