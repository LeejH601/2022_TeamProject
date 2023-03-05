#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

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

Texture2D gtxMappedTexture[7] : register(t0);
SamplerState gSamplerState : register(s0);

#include "Light.hlsl"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD0;
	float4 uvs[MAX_LIGHTS] : TEXCOORD1;
};

struct CB_TOOBJECTSPACE
{
	matrix		mtxToTexture;
	float4		f4Position;
};

cbuffer cbToLightSpace : register(b6)
{
	CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};

[earlydepthstencil]
float4 PS_Tree(VS_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float2 uv_MainTexture = input.uv * float2(1, 1) + float2(0, 0);
	//float4 tex2DNode2 = tex2D(_MainTexture, uv_MainTexture);
	

	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxMappedTexture[0].Sample(gSamplerState, uv_MainTexture);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxMappedTexture[2].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxMappedTexture[3].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxMappedTexture[4].Sample(gSamplerState, input.uv);

	/*float3 NormalW = input.normalW;
	NormalW.x = -NormalW.x;
	NormalW.y = -NormalW.y;
	NormalW.z = -NormalW.z;*/

	//float3 NormalW = float3(input.normalW.x, -input.normalW.z, input.normalW.y);


	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW);
	//float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	//float3 B = cross(N, T);
	float3 B = normalize(input.bitangentW);

	float3x3 TBN = float3x3(T, B, N);

	float3 normal = cNormalColor.rgb;
	//float3 normal = float3(cNormalColor.r, -cNormalColor.b, cNormalColor.g);
	normal = (2.0f * normal) - 1.0f;
	/*normal.y = -normal.y;
	normal.z = -normal.z;*/

	float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
	cColor = (cColor * float4(1.0f, 1.0f, 1.0f, 1.0f);

	/*if (gnTexturesMask & MATERIAL_METALLIC_MAP) {
		float MetailcConstant = cMetallicColor.r * 0.1f;
		float4 MetalColor = float4(0.95f, 0.95f, 0.95f, 1.0f);
		cColor = lerp(cColor, MetalColor, MetailcConstant);
	}
	 */

		 //float3 normalW = normalize(input.normalW);
		 float3 normalW = mul(normal, TBN);

		 float4 cIllumination;
		 if (gnTexturesMask & MATERIAL_NORMAL_MAP)
			 cIllumination = Lighting(input.positionW, normalize(normalW), cColor, true, input.uvs);
		 else
			 cIllumination = Lighting(input.positionW, normalize(input.normalW), cColor, true, input.uvs);
		 //float4 cIllumination = Lighting(input.positionW, normalize(input.normalW), cColor, true, input.uvs);

		 //return float4(1.0f, 0.0f, 0.0f, 1.0f);
		 return (cIllumination);
}