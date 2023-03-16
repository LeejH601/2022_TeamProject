#include "Header.hlsli"

SamplerState gSamplerState : register(s0);
Texture2D gtxtTexture : register(t30);

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

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

#include "Light.hlsl"

#define radius 0.02f

PS_MULTIPLE_RENDER_TARGETS_OUTPUT SunLight_PS(VS_SCREEN_RECT_TEXTURED_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;



	//float3 uvw = float3(input.uv, input.primID % 4); // 수정 float3 uvw = float3(input.uv, input.primID ); 

	////float4 cTexture = gtxtTexture.Sample(gSamplerState, uvw);
	////float4 cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//float4 cColor = cTexture; //  // cIllumination * cTexture;

	//if (cColor.a > 0.0f) {
	//	output.f4Illumination = cColor;
	//	//output.f4Color = cColor;
	//}

	///*output.f4Illumination = cColor;
	//output.f4Color = cColor;*/

	float4 lightPosInViewSpace = mul(float4(gLights[0].m_vPosition, 1.0), gmtxView);
	float4 lightPosInClipSpace = mul(lightPosInViewSpace, gmtxProjection);
	float3 lightPosInNDC = lightPosInClipSpace.xyz / lightPosInClipSpace.w;

	lightPosInNDC.x = min(1.0f, max(-1.0f, lightPosInNDC.x));
	lightPosInNDC.y = min(1.0f, max(-1.0f, lightPosInNDC.y));

	float2 lightPosInScreenSpace = float2(lightPosInNDC.x * 0.5 + 0.5, -lightPosInNDC.y * 0.5 + 0.5);

	float result = pow((input.uv.x - lightPosInScreenSpace.x), 2) + pow((input.uv.y - lightPosInScreenSpace.y), 2);
	float radiusPow = pow(radius, 2);

	float4 cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (result < radiusPow) {
		output.f4Illumination = cColor;
		//output.f4Color = cColor;
	}
	else {
		//output.f4Illumination = float4(0.1f, 0.1f, 0.1f, 1.0f);
		output.f4Illumination = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	return(output);
}