
#include "Header.hlsli"

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

cbuffer cbParallax : register(b5)
{
	float gfParallaxScale;
	float gfParallaxBias;
	int gfMappingMode;
}

Texture2D gtxMappedTexture[7] : register(t0);

Texture2D gtxtTerrainTexture : register(t0);
Texture2D gtxtDetailTexture : register(t1);
Texture2D gtxtAlphaTexture : register(t2);


Texture2D gtxTerrainTextures[13] : register(t10);


SamplerState gSamplerState : register(s0);

#include "Light.hlsl"

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 positionW : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 normalW : NORMAL1;
	float3 tangent : TANGENT;
	float3 toCamera : TEXCOORD2;
	float3 toLight : TEXCOORD3;
	float4 uvs[MAX_LIGHTS] : TEXCOORD4;
};


static int gnMaxSamples = 20;
static int gnMinSamples = 4;
static float gfscale = 0.0001f;

//#define USE_PARALLAX

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParallaxTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float fParallaxLimit = -length(input.toCamera.xy) / input.toCamera.z;
	fParallaxLimit *= gfscale;

	float2 vOffsetDir = normalize(input.toCamera);
	float2 vMaxOffset = vOffsetDir * fParallaxLimit;

	int nSamples = (int)lerp(gnMaxSamples, gnMinSamples, dot(input.toCamera, input.normal));

	float fStepSize = 1.0f / (float)nSamples;

	float2 dx = ddx(input.uv0);
	float2 dy = ddy(input.uv0);

	float fCurryRayHeight = 1.0f;
	float2 vCurrOffset = float2(0.0f, 0.0f);
	float2 vLastOffset = float2(0.0f, 0.0f);

	float fLastSampleHeight = 1.0f;
	float fCurrSampleHeight = 1.0f;

	int nCurrSample = 0;


	float4 cAlphaColor = gtxTerrainTextures[12].Sample(gSamplerState, input.uv1);
	float4 cNormalColor[4];
	float4 cHeightColor[4];
	float4 FHeightColor;
	/*int HighestAlpha = 0;
	for (int i = 1; i < 4; ++i) {
		if (cAlphaColor[HighestAlpha] < cAlphaColor[i])
			HighestAlpha = i;
	}*/


	float2 vFinalCoords;


#ifdef USE_PARALLAX
	[unroll(20)]
	while (nCurrSample < nSamples) {
		for (int i = 0; i < 4; ++i) {
			cHeightColor[i] = gtxTerrainTextures[8 + i].SampleGrad(gSamplerState, input.uv0 + vCurrOffset, dx, dy);

			if (i == 0 && cAlphaColor.r > 0.0f) {
				FHeightColor += cHeightColor[i] * cAlphaColor.r;
			}
			else if (i == 1 && cAlphaColor.g > 0.0f) {
				FHeightColor += cHeightColor[i] * cAlphaColor.g;
			}
			else if (i == 2 && cAlphaColor.b > 0.0f) {
				FHeightColor += cHeightColor[i] * cAlphaColor.b;
			}
			else if (i == 3 && cAlphaColor.a > 0.0f) {
				FHeightColor += cHeightColor[i] * cAlphaColor.a;
			}
		}

		fCurrSampleHeight = FHeightColor.r;
		if (fCurrSampleHeight > fCurryRayHeight) {
			float fDelta1 = fCurrSampleHeight - fCurryRayHeight;
			float fDelta2 = (fCurryRayHeight + fStepSize) - fLastSampleHeight;
			float fRatio = fDelta1 / (fDelta1 + fDelta2);
			vCurrOffset = (fRatio)*vLastOffset + (1.0f - fRatio) * vCurrOffset;
			nCurrSample = nSamples + 1;
		}
		else {
			nCurrSample++;
			fCurryRayHeight -= fStepSize;
			vLastOffset = vCurrOffset;
			vCurrOffset += fStepSize * vMaxOffset;
			fLastSampleHeight = fCurrSampleHeight;
		}
	}

	vFinalCoords = input.uv0 + vCurrOffset;
#endif

	vFinalCoords = input.uv0;
	float4 vFinalNormal;
	float4 vFinalColor;
	float4 cBaseTexColor[4];

	for (int i = 0; i < 4; ++i) {
		cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, vFinalCoords);
		cNormalColor[i] = gtxTerrainTextures[4 + i].Sample(gSamplerState, vFinalCoords);

		if (i == 0 && cAlphaColor.r > 0.0f) {
			vFinalColor += cBaseTexColor[i] * cAlphaColor.r;
				vFinalNormal += cNormalColor[i] * cAlphaColor.r;
		}
		else if (i == 1 && cAlphaColor.g > 0.0f) {
			vFinalColor += cBaseTexColor[i] * cAlphaColor.g;
				vFinalNormal += cNormalColor[i] * cAlphaColor.g;
		}
		else if (i == 2 && cAlphaColor.b > 0.0f) {
			vFinalColor += cBaseTexColor[i] * cAlphaColor.b;
				vFinalNormal += cNormalColor[i] * cAlphaColor.b;
		}
		else if (i == 3 && cAlphaColor.a > 0.0f) {
			vFinalColor += cBaseTexColor[i] * cAlphaColor.a;
				vFinalNormal += cNormalColor[i] * cAlphaColor.a;
		}
	}


	float3x3 mtxTangentToWorld;
	mtxTangentToWorld[0] = normalize(input.tangent);
	mtxTangentToWorld[2] = normalize(input.normal);
	mtxTangentToWorld[1] = normalize(cross(input.tangent, input.normal));

	vFinalNormal = vFinalNormal * 2.0f - 1.0f;
	float3 toLight = float3(0.0f, 1.0f, 0.0f);
	float3 Normal = mul(vFinalNormal, mtxTangentToWorld);
	/*float3 vAmbient = vFinalColor.rgb * 0.2f;
	vFinalColor.rgb = vFinalColor.rgb + vAmbient;*/
	vFinalColor.rgb = vFinalColor.rgb * 2.0f;
	float4 cIllumination = Lighting(input.positionW, Normal, vFinalColor, true, input.uvs);
	//float4 cIllumination = Lighting(input.positionW, normalize(Normal), vFinalColor);

	/*float3 vAmbient = vFinalColor.rgb * 1.0f;
	cIllumination = cIllumination + float4(vAmbient, 0.0f);*/

	//float3 vDiffuse = (lerp(vFinalColor, cIllumination, 0.2f).xyz);
	//float3 vDiffuse = vFinalColor.rgb * max(0.0f, dot(input.toLight, vFinalNormal.xyz)) * 0.5;
	//vFinalColor.rgb = vAmbient + vDiffuse;

	output.f4Scene = cIllumination;
	output.f4Color = vFinalColor;
	float Depth = cIllumination.w < 0.001f ? 0.0f : input.position.z;
	output.f4Normal = float4(Normal * 0.5f + 0.5f, 0.0f);
	output.f4PositoinW = input.positionW;

	float fShadowFactor = gtxtDepthTextures[0].SampleCmpLevelZero(gssComparisonPCFShadow, input.uvs[0].xy / input.uvs[0].ww, input.uvs[0].z / input.uvs[0].w).r;
	output.f4Illumination = float4(0.0f, 0.0f, 0.0f, 1.0f);
	/*if (fShadowFactor > 0.0f)
		output.f4Illumination = float4(1.0f, 1.0f, 1.0f, 0.0f);
	else
		output.f4Illumination = float4(0.0f, 0.0f, 0.0f, 1.0f);*/

	return (output);
}