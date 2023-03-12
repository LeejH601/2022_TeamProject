#include "Header.hlsli"
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define MAX_OJBECT_DISSOLVE 250

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8.x);
	uint gnInstanceID : packoffset(c8.y);
	//float gfDissolveThreshHold : packoffset(c8.y);
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
//
//cbuffer cbParallax : register(b5)
//{
//	bool gbDissolved;
//	float gfDissolveThreshHold;
//}



cbuffer cbDissolveParam : register(b5)
{
	float4 gfDissolveThreshHold[MAX_OJBECT_DISSOLVE] : packoffset(c0);
}

Texture2D gtxMappedTexture[8] : register(t0);
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


#define STEP 0.15f
#define INTERVEL_STEPS 0.05f

float4 CalculateDissolve(float4 color, float2 uv, float ThreshHold) {
	float4 NoiseColor = gtxMappedTexture[2].Sample(gSamplerState, uv / 16 + 0.5f);

	float step2 = STEP + INTERVEL_STEPS;
	float Alpha = 1.0f;

	if (ThreshHold <= 0.0f)
		return color;

	if (NoiseColor.r <= ThreshHold) {
		float4 emissionColor = float4(0.3125, 0.734f, 0.871f, 0.6f);
		color = emissionColor;
	}
	if (NoiseColor.r <= ThreshHold - STEP) {
		float emissionAlpha = 0.2f;
		float AlphaWeight = 0.f;
		if ((ThreshHold - STEP) >= 1.0f - INTERVEL_STEPS) {
			emissionAlpha = emissionAlpha - ((ThreshHold - STEP) - (1.0f - INTERVEL_STEPS)) * 4.0f;
		}
		float4 emissionColor = float4(0.117, 0.562, 1.0f, emissionAlpha);
		color = emissionColor;
	}
	if (NoiseColor.r <= ThreshHold - step2) {
		color.a = 0.0f;
	}
	return color;
}

//[earlydepthstencil]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Player(VS_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);


	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxMappedTexture[0].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
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


	cColor = CalculateDissolve(cColor, input.uv, gfDissolveThreshHold[gnInstanceID / 4][gnInstanceID % 4]);
	//cIllumination = CalculateDissolve(cIllumination, input.uv, gfDissolveThreshHold[gnInstanceID / 4][gnInstanceID % 4]);

	output.f4Scene = cIllumination;
	if (cColor.a > 0.001f) {
		output.f4Color = cColor;
		output.f4PositoinW = float4(input.positionW, 1.0f);
		float Depth = cIllumination.w < 0.001f ? 0.0f : input.position.z;
		output.f4Normal = float4(input.normalW.xyz * 0.5f + 0.5f, Depth);
	}
	else
		discard;

	float gray = dot(cIllumination.rgb, float3(0.299, 0.587, 0.114));
	/* if (gray > 1.0)
		 output.f4Color = cIllumination;
	 else
		 output.f4Color = float4(0.0f, 0.0f, 0.0f, 1.0f);*/

	//output.f4Texture.x = Compute3x3ShadowFactor(input.uvs[0].xy / input.uvs[0].ww, input.uvs[0].z / input.uvs[0].w, 0);


	//output.f4Texture.x = Compute3x3ShadowFactor(input.uvs[0].xy / input.uvs[0].ww, input.uvs[0].z / input.uvs[0].w, 0);

	//return float4(gfDissolveThreshHold[0], 0.0f, 0.0f, 1.0f);
	return (output);
}

