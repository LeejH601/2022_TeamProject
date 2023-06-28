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
	uint gnRimLightEnable : packoffset(c8.z);
	//float gfDissolveThreshHold : packoffset(c8.y);
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
	//float4 uvs[MAX_LIGHTS] : TEXCOORD1;
	uint instanceID : SV_InstanceID;
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


cbuffer VS_CB_RIMLIGHT_INFO : register(b8)
{
	float m_gfRimLightFactor;
	float3 m_gxmf3RimLightColor;
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
		emissionColor.xyz *= 2.0f;
		color = emissionColor;
	}
	if (NoiseColor.r <= ThreshHold - STEP) {
		float emissionAlpha = 0.2f;
		float AlphaWeight = 0.f;
		if ((ThreshHold - STEP) >= 1.0f - INTERVEL_STEPS) {
			emissionAlpha = emissionAlpha - ((ThreshHold - STEP) - (1.0f - INTERVEL_STEPS)) * 4.0f;
		}
		float4 emissionColor = float4(0.117, 0.562, 1.0f, emissionAlpha);
		emissionColor.xyz *= 2.0f;
		color = emissionColor;
	}
	if (NoiseColor.r <= ThreshHold - step2) {
		color.a = 0.0f;
	}
	return color;
}

[earlydepthstencil]
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Player(VS_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxMappedTexture[0].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxMappedTexture[1].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxMappedTexture[3].Sample(gSamplerState, input.uv);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxMappedTexture[4].Sample(gSamplerState, input.uv);

	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW);
	float3 B = normalize(input.bitangentW);

	float3x3 TBN = float3x3(T, B, N);

	float3 normal = cNormalColor.rgb;
	normal = (2.0f * normal) - 1.0f;

	float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
	//cColor.xyz *= 1.5f;s

	float3 normalW = mul(normal, TBN);


	cColor = CalculateDissolve(cColor, input.uv, gfDissolveThreshHold[gnInstanceID / 4][gnInstanceID % 4]);

	if (cColor.a > 0.001f) {
		float3 toCameraVec = normalize(gf3CameraPosition - input.positionW.xyz);
		float RimLight = smoothstep(1.0f - 0.9, 1.0f, 1 - max(0, dot(normalize(input.normalW), toCameraVec)));
		float3 RimLightColor = float3(2.5f,0.0f,0.0f);
		RimLight *= float(gnRimLightEnable);

		cColor.xyz += RimLightColor * RimLight;

		output.f4Color = cColor;
		output.f4PositoinW = float4(input.positionW, 1.0f);
		float Depth = cColor.w < 0.001f ? 0.0f : input.position.z;
		if (gnTexturesMask & MATERIAL_NORMAL_MAP)
			output.f4Normal = float4(normalW * 0.5f + 0.5f, Depth);
		else
			output.f4Normal = float4(input.normalW.xyz * 0.5f + 0.5f, Depth);
		//output.f4Illumination = float4(1.0f, 1.0f, 1.0f, 1.0f);

		//float fShadowFactor = gtxtDepthTextures[0].SampleCmpLevelZero(gssComparisonPCFShadow, input.uvs[0].xy / input.uvs[0].ww, input.uvs[0].z / input.uvs[0].w).r;
		output.f4Illumination = float4(0.0f, 0.0f, 0.0f, 1.0f);
		/*if(fShadowFactor > 0.0f )
			output.f4Illumination = float4(1.0f, 1.0f, 1.0f, input.instanceID);
		else
			output.f4Illumination = float4(0.0f, 0.0f, 0.0f, 1.0f);*/
	}
	else
		discard;

	return (output);
}

