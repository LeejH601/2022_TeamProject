


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

#define NUM_SAMPLES 128

float4 LightShaft(float2 uv, float2 ScreenLightPos, float Weight, float Exposure, float Density, float Decay)
{
	float2 DeltaUV = (uv - ScreenLightPos.xy);
	DeltaUV *= 1.0f / (float)NUM_SAMPLES * Density;


	float4 Basecolor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv);
	float3 color = float3(0.0f, 0.0f, 0.0f);
	//float3 color = Basecolor.xyz;
	//float illuminationDecay = 0.5f;
	float illuminationDecay = 1.0f;

	float2 TexCoord = uv - (DeltaUV * NUM_SAMPLES);
	float4 OcculusionColor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv);

	/*if (OcculusionColor.r < 0.0001f) {
		float3 grayscale = dot(color, float3(0.3, 0.59, 0.11));
		if(grayscale.r < 0.6f)
			Basecolor = lerp(Basecolor, OcculusionColor, 0.5f);
		return Basecolor;
	}*/

	//uv += DeltaUV * NUM_SAMPLES;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		uv -= DeltaUV;
		float3 sampleColor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv).xyz;
		//sampleColor = 1.0f - sampleColor;

		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;

		illuminationDecay *= Decay;
	}

	return float4(color * Exposure, Basecolor.w);
}

//float VolumeticFog(float3 worldPos) {
//	float4 mainPos = mul(float4(worldPos, 1.0f), gcbToLightSpaces[0].mtxToTexture);
//	float4 startPos = mul(float4(gf3CameraPosition, 1.0f), gcbToLightSpaces[0].mtxToTexture);
//	float4 LightPos = mul(gcbToLightSpaces[0].f4Position, gcbToLightSpaces[0].mtxToTexture);
//
//	float2 uv = (mainPos.xy / mainPos.ww);
//	float2 LightPosInDepth = float2(LightPos.xy / LightPos.ww);
//
//	float4 Basecolor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, uv);
//	float3 color = Basecolor.xyz;
//
//	float illuminationDecay = 1.0f;
//
//	for (int i = 0; i < NUM_SAMPLES; i++) {
//		uv -= DeltaUV;
//		float3 sampleColor = gtxMultiRenderTargetTextures[3].Sample(gSamplerState, uv).xyz;
//		sampleColor = 1.0f - sampleColor;
//
//		sampleColor *= illuminationDecay * Weight;
//		color += sampleColor;
//		//color = sampleColor;
//
//		illuminationDecay *= Decay;
//	}
//
//	//float2 DeltaUV = (uv - ScreenLightPos.xy);
//}

//void executeRaymarching(float3 VLI, float3 rayPositionLight, float stepSize, float3 invViewDir) {
//	rayPositionLight += stepSize * invViewDir;
//
//	float3 shadowFactor = fShadowFactor = gtxtDepthTextures[0].SampleCmpLevelZero(gssComparisonPCFShadow, uvs[i].xy / uvs[i].ww, uvs[i].z / uvs[i].w).r;
//}
//
//
//#define NUM_SAMPLES_RCP 0.007125
//
//float Volumetirc(float4 cColor) {
//	float rayMarchDistanceLimit = 999999.0f;
//	
//	//...
//
//	float rayMarchDistance = trunc(clamp(length(cameraPosLight.xyz - posLight.xyz), 0.0, rayMarchDistanceLimit);
//
//	float stepSize = rayMarchDistance * NUM_SAMPLES_RCP;
//	float3 rayPositionLight = posLight.xyz;
//
//	float3 VLI 0.0;
//
//	[loop] for (float l = rayMarchDistance; l > stepSize; l -= stepSize) {
//		executeRaymarching(VLI);
//	}
//
//	float4 color = float4(cColor.xyz * VLI, cColor.w);
//	return color;
//}

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

	lightPosInNDC.x = min(1.0f, max(-1.0f, lightPosInNDC.x));
	lightPosInNDC.y = min(1.0f, max(-1.0f, lightPosInNDC.y));

	float2 lightPosInScreenSpace = float2(lightPosInNDC.x * 0.5 + 0.5, -lightPosInNDC.y * 0.5 + 0.5);

	float result = pow((input.uv.x - lightPosInScreenSpace.x), 2) + pow((input.uv.y - lightPosInScreenSpace.y), 2);

	// LightShaft Values
	float weight = 0.7f;
	float exposure = 0.6f;
	float Density = 2.0f;
	float Decay = 0.95f;
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
	/*if (result < radiusPow) {
		weight *= max(0.0f, (1.0f - ((result) / radiusPow)));
		Density *= max(0.0f, (1.0f - ((result) / radiusPow)));
	}
	else {
		weight = 0.0f;
	}*/

	//Density *= (1.0f - (result / radiusPow));
	float angle = dot(normalize(gLights[0].m_vDirection),  normalize(Pixelnormal.xyz));
	/*if (angle > 0)
		exposure = 0.6f;*/

	float4 uvs[MAX_LIGHTS];

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gcbToLightSpaces[i].f4Position.w != 0.0f) uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
	}

	cColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerState, input.uv);

	float4 ShaftColor;
	ShaftColor = LightShaft(input.uv, lightPosInScreenSpace, weight, exposure, Density, Decay);

	//
	//if (lightPosInClipSpace.z > 0.0f) {
	//	//Density *= (1.0f - lightPosInClipSpace.z);e
	//	if (!(lightPosInNDC.z > 1.0f))
	//		ShaftColor = LightShaft(input.uv, lightPosInScreenSpace, weight, exposure, Density, Decay);
	//}
	//else
	//	ShaftColor = LightShaft(input.uv, lightPosInScreenSpace, weight, exposure, 0.0, Decay);

	cColor.xyz += ShaftColor.xyz;

	float3 normalOffsetScale = 20.0f;
	float3 normalOffset = normalOffsetScale * normal;
	float3 normalOffsetedPosition = positionW.xyz + normalOffset;

	cColor = Lighting(normalOffsetedPosition, normalize(normal), cColor, true, uvs);


	return cColor;
}