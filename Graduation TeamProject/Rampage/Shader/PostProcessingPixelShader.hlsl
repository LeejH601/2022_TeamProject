
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

#define NUM_SAMPLES 128
#define Decay 0.9f

float4 LightShaft(float2 uv, float2 ScreenLightPos, float Weight, float Exposure, float Density)
{
	float2 DeltaUV = (uv - ScreenLightPos.xy);
	DeltaUV *= 1.0f / NUM_SAMPLES * Density;

	float4 Basecolor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, uv);
	float3 color = Basecolor.xyz;
	float illuminationDecay = 1.0f;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		uv -= DeltaUV;
		float3 sampleColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, uv).xyz;
		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;
		illuminationDecay *= Decay;
	}

	return float4(color * Exposure, Basecolor.w);
}

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

#define radius 1.0f

float4 PS_PostProcessing(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
	// gLights[0]에 들어있는 직선광을 스크린 공간 좌표계로 변환
	float4 lightPosInViewSpace = mul(float4(gLights[0].m_vPosition, 1.0), gmtxView);
	float4 lightPosInClipSpace = mul(lightPosInViewSpace, gmtxProjection);
	float3 lightPosInNDC = lightPosInClipSpace.xyz / lightPosInClipSpace.w;
	float2 lightPosInScreenSpace = float2(lightPosInNDC.x * 0.5 + 0.5, -lightPosInNDC.y * 0.5 + 0.5);

	float result = pow((input.uv.x - lightPosInScreenSpace.x), 2) + pow((input.uv.y - lightPosInScreenSpace.y), 2);


	//float4 cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);
	//float4 cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

	float weight = 0.1f;
	float exposure = 0.8f;
	float Density = 0.7f;
	float radiusPow = pow(radius, 2);
	float4 cColor;
	float shadowFactor = gtxMultiRenderTargetTextures[2].Sample(gSamplerState, input.uv).x;
	float4 Pixelnormal = gtxMultiRenderTargetTextures[1].Sample(gSamplerState, input.uv);


	// 광원이 범위 안쪽에 위치하고, 카메라가 바라보는 방향에 있다면 효과를 적용
	//if (/*result < radiusPow && */!(lightPosInNDC.z > 1.0f)) {
	if ((lightPosInNDC.z > 1.0f) || Pixelnormal.z > 0.0f)
		weight = 0.0f;

	if(result < radiusPow)
		weight *= max(0.0f, (1.0f - ((result) / radiusPow)));
	else
		weight = 0.0f;

		Density *= (1.0f - (result / radiusPow) );
		//exposure = max(0.4, shadowFactor);
		float angle = dot(normalize(gLights[0].m_vDirection),  normalize(Pixelnormal.xyz));
		if (angle > 0)
			exposure = 0.6f;
		cColor = LightShaft(input.uv, lightPosInScreenSpace, weight, exposure, Density);
	//}
	//else
	//	cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);

	return cColor;
}