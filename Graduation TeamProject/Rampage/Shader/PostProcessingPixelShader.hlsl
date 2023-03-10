
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
};

Texture2D gtxMultiRenderTargetTextures[7] : register(t35);
SamplerState gSamplerState : register(s0);

#include "Light.hlsl"

#define NUM_SAMPLES 64
#define Density 0.6f
//#define Weight 0.2f
#define Decay 0.7f
#define Exposure 1.0f

float4 LightShaft(float2 uv, float2 ScreenLightPos, float Weight)
{
	float2 DeltaUV = (uv - ScreenLightPos.xy);
	DeltaUV *= 1.0f / NUM_SAMPLES * Density;

	float3 color = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, uv);
	float illuminationDecay = 1.0f;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		uv -= DeltaUV;
		float3 sampleColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, uv);
		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;
		illuminationDecay *= Decay;
	}

	return float4(color * Exposure, 1);
}

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

#define radius 0.7f

float4 PS_PostProcessing(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
	float4 lightPosInViewSpace = mul(float4(gLights[0].m_vPosition, 1.0), gmtxView);
	float4 lightPosInClipSpace = mul(lightPosInViewSpace, gmtxProjection);
	float3 lightPosInNDC = lightPosInClipSpace.xyz / lightPosInClipSpace.w;
	float2 lightPosInScreenSpace = float2(lightPosInNDC.x * 0.5 + 0.5, -lightPosInNDC.y * 0.5 + 0.5);

	float result = pow((input.uv.x - lightPosInScreenSpace.x), 2) + pow((input.uv.y - lightPosInScreenSpace.y), 2);


	//float4 cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);
	//float4 cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	float weight = 0.4f;
	float radiusPow = pow(radius, 2);
	float4 cColor;

	if (lightPosInNDC.z > 1.0f) {
		cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);
		return cColor;
	}

	if (result < radiusPow) {
		weight *= (1.0f - (result / radiusPow));
		cColor = LightShaft(input.uv, lightPosInScreenSpace, weight);
	}
	else
		cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);

	return cColor;
}