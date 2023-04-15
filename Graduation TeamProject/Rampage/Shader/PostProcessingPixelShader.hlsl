


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

struct CB_TOOBJECTSPACE
{
	matrix		mtxToTexture;
	float4		f4Position;
};

cbuffer cbToLightSpace : register(b6)
{
	CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};

#define NUM_SAMPLES 90

float3 LightShaft(float2 uv, float2 ScreenLightPos, float Weight, float Exposure, float Density, float Decay)
{
	float2 DeltaUV = (uv - ScreenLightPos.xy);
	DeltaUV *= 1.0f / (float)NUM_SAMPLES * Density;


	//float4 Basecolor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv);
	float3 color = float3(0.0f, 0.0f, 0.0f);
	float illuminationDecay = 1.0f;

	float2 TexCoord = uv - (DeltaUV * NUM_SAMPLES);
	float4 OcculusionColor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv);

	for (int i = 0; i < NUM_SAMPLES; i++) {
		uv -= DeltaUV;
		float3 sampleColor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv).xyz;
		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;

		illuminationDecay *= Decay;
	}

	return color * Exposure;
}


struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

#define radius 1.0f

float4 PS_PostProcessing(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target1
{
	// gLights[0]에 들어있는 직선광을 스크린 공간 좌표계로 변환
	float4 lightPosInViewSpace = mul(float4(gLights[0].m_vPosition, 1.0), gmtxView);
	float4 lightPosInClipSpace = mul(lightPosInViewSpace, gmtxProjection);
	float3 lightPosInNDC = lightPosInClipSpace.xyz / lightPosInClipSpace.w;

	float2 lightPosInScreenSpace = float2(lightPosInNDC.x * 0.5 + 0.5, -lightPosInNDC.y * 0.5 + 0.5);

	float result = pow((input.uv.x - lightPosInScreenSpace.x), 2) + pow((input.uv.y - lightPosInScreenSpace.y), 2);

	// LightShaft Values
	float weight = 0.8f;
	float exposure = 0.6f;
	float Density = 1.0f;
	float Decay = 0.85f;
	float radiusPow = pow(radius, 2);

	float4 cColor;
	float4 Pixelnormal = gtxMultiRenderTargetTextures[1].Sample(gSamplerState, input.uv);
	float3 normal = Pixelnormal.xyz * 2.0f - 1.0f;
	float4 positionW = gtxMultiRenderTargetTextures[2].Sample(gSamplerState, input.uv);




	// 광원이 범위 안쪽에 위치하고, 카메라가 바라보는 방향에 있다면 효과를 적용
	//if ((lightPosInNDC.z > 1.0f) || Pixelnormal.z > 0.0f) {
	//	//weight = 0.0f;
	//	Density = 0.0f;
	//}

	// 광원을 기준으로 원의 방정식 상에서 점의 위치에 따라 weigth 값을 조절
	// 범위를 벗어나면 weight를 0으로 고정
	if (result < radiusPow) {
		weight *= max(0.0f, (1.0f - ((result) / radiusPow)));
	}
	else {
		weight = 0.0f;
	}

	float angle = dot(normalize(gLights[0].m_vDirection),  normalize(Pixelnormal.xyz));

	float4 uvs[MAX_LIGHTS];

	float3 normalOffsetScale = 0.15f;
	float3 normalOffset = normalOffsetScale * normal;
	float4 normalOffsetedPosition = positionW;
	normalOffsetedPosition.xyz += normalOffset;

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gcbToLightSpaces[i].f4Position.w != 0.0f) uvs[i] = mul(normalOffsetedPosition, gcbToLightSpaces[i].mtxToTexture);
	}

	cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);

	float3 ShaftColor;
	ShaftColor = LightShaft(input.uv, lightPosInScreenSpace, weight, exposure, Density, Decay);
	cColor.xyz += ShaftColor.xyz;

	cColor = Lighting(positionW.xyz, normal, cColor, true, uvs);
	float4 trailColor = gtxMultiRenderTargetTextures[4].Sample(gSamplerState, input.uv);
	cColor += trailColor;

	return cColor;
}