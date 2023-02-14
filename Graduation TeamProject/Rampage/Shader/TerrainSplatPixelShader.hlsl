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
	float3 positionW : POSITIONl;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer
	float4 f4ImGui : SV_TARGET1;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrainSplat(VS_TERRAIN_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cBaseTexColor[4];
	float4 cNormalColor[4];
	float4 cHeightColor[4];
	int index = 8;
	float4 cAlphaColor = gtxTerrainTextures[12].Sample(gSamplerState, input.uv1);

	float4 cColor;
	float4 FNormalColor;
	float4 FHeightColor;
	
	if (gfMappingMode == 0) {
		for (int i = 0; i < 4; ++i) {
			/*cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, input.uv0);
			cNormalColor[i] = gtxTerrainTextures[4+i].Sample(gSamplerState, input.uv0);*/
			cHeightColor[i] = gtxTerrainTextures[8 + i].Sample(gSamplerState, input.uv0);

			if (i == 0) {
				//cColor += cBaseTexColor[i] * cAlphaColor.r;
				//FNormalColor += cNormalColor[i] * cAlphaColor.r;
				FHeightColor += cHeightColor[i] * cAlphaColor.r;
			}
			else if (i == 1) {
				//cColor += cBaseTexColor[i] * cAlphaColor.g;
				//FNormalColor += cNormalColor[i] * cAlphaColor.g;
				FHeightColor += cHeightColor[i] * cAlphaColor.g;
			}
			else if (i == 2) {
				//cColor += cBaseTexColor[i] * cAlphaColor.b;
				//FNormalColor += cNormalColor[i] * cAlphaColor.b;
				FHeightColor += cHeightColor[i] * cAlphaColor.b;
			}
			else if (i == 3) {
				//cColor += cBaseTexColor[i] * cAlphaColor.a;
				//FNormalColor += cNormalColor[i] * cAlphaColor.a;
				FHeightColor += cHeightColor[i] * cAlphaColor.a;
			}
		}

		float3 vCameraPosition = gf3CameraPosition;
		float3 vToCamera = normalize(vCameraPosition - input.positionW);

		float fHeight = FHeightColor.r;
		fHeight = fHeight * gfParallaxScale + gfParallaxBias;
		float2 vTexCorrected = input.uv0 + fHeight * vToCamera.xy;

		for (int i = 0; i < 4; ++i) {
			cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, vTexCorrected);
			cNormalColor[i] = gtxTerrainTextures[4 + i].Sample(gSamplerState, vTexCorrected);

			if (i == 0) {
				cColor += cBaseTexColor[i] * cAlphaColor.r;
				FNormalColor += cNormalColor[i] * cAlphaColor.r;
			}
			else if (i == 1) {
				cColor += cBaseTexColor[i] * cAlphaColor.g;
				FNormalColor += cNormalColor[i] * cAlphaColor.g;
			}
			else if (i == 2) {
				cColor += cBaseTexColor[i] * cAlphaColor.b;
				FNormalColor += cNormalColor[i] * cAlphaColor.b;
			}
			else if (i == 3) {
				cColor += cBaseTexColor[i] * cAlphaColor.a;
				FNormalColor += cNormalColor[i] * cAlphaColor.a;
			}
		}


		float3 N = normalize(input.normalW);
		float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
		float3 B = cross(N, T);
		float3x3 TBN = float3x3(T, B, N);

		float3 normal = FNormalColor.rgb;
		normal = 2.0f * normal - 1.0f;

		float3 normalW = mul(normal, TBN);
		float4 cIllumination = Lighting(input.positionW, normalW);
		//float4 cIllumination = Lighting(input.position, input.normalW);
		cColor = lerp(cColor, cIllumination, 0.2f);
	}
	else if (gfMappingMode == 1) {
		for (int i = 0; i < 4; ++i) {
			cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, input.uv0);
			cNormalColor[i] = gtxTerrainTextures[4 + i].Sample(gSamplerState, input.uv0);

			if (i == 0) {
				cColor += cBaseTexColor[i] * cAlphaColor.r;
				FNormalColor += cNormalColor[i] * cAlphaColor.r;
			}
			else if (i == 1) {
				cColor += cBaseTexColor[i] * cAlphaColor.g;
				FNormalColor += cNormalColor[i] * cAlphaColor.g;
			}
			else if (i == 2) {
				cColor += cBaseTexColor[i] * cAlphaColor.b;
				FNormalColor += cNormalColor[i] * cAlphaColor.b;
			}
			else if (i == 3) {
				cColor += cBaseTexColor[i] * cAlphaColor.a;
				FNormalColor += cNormalColor[i] * cAlphaColor.a;
			}
		}

		float3 N = normalize(input.normalW);
		float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
		float3 B = cross(N, T);
		float3x3 TBN = float3x3(T, B, N);

		float3 normal = FNormalColor.rgb;
		normal = 2.0f * normal - 1.0f;

		float3 normalW = mul(normal, TBN);
		float4 cIllumination = Lighting(input.positionW, normalW);
		//float4 cIllumination = Lighting(input.position, input.normalW);
		cColor = lerp(cColor, cIllumination, 0.2f);
	}
	else if (gfMappingMode == 2) {
		for (int i = 0; i < 4; ++i) {
			cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, input.uv0);

			if (i == 0) {
				cColor += cBaseTexColor[i] * cAlphaColor.r;
			}
			else if (i == 1) {
				cColor += cBaseTexColor[i] * cAlphaColor.g;
			}
			else if (i == 2) {
				cColor += cBaseTexColor[i] * cAlphaColor.b;
			}
			else if (i == 3) {
				cColor += cBaseTexColor[i] * cAlphaColor.a;
			}
		}

		float4 cIllumination = Lighting(input.positionW, input.normalW);
		//float4 cIllumination = Lighting(input.position, input.normalW);
		cColor = lerp(cColor, cIllumination, 0.2f);
	}
	

	output.f4Scene = cColor;
	output.f4ImGui = cColor;

	return output;
}