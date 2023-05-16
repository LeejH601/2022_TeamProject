#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Buffer<float4> gRandomSphereBuffer : register(t50);
Texture2D gtxtTexture[29] : register(t51);
SamplerState gClampState : register(s4);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
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

#include "Light.hlsl"

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	uint primID : SV_PrimitiveID;
	//int textureIndex : TEXTUREINDEX;
};


PS_MULTIPLE_RENDER_TARGETS_OUTPUT Billboard_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float3 uvw = float3(input.uv, input.primID % 4); // 수정 float3 uvw = float3(input.uv, input.primID ); 
	float4 cTexture = gtxtTexture[((uint)(gmtxTexture._11 - 1))].Sample(gClampState, uvw.xy); // SphereTexture t50을 제외
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	cColor.rgb *= gfColor * 3.5f;
	cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	output.f4Scene = cColor;
	output.f4Color = cColor;

	return(output);
}