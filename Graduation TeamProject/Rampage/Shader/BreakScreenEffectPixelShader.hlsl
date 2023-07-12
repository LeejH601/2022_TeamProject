#include "Header.hlsli"
#include "CurlNoise.hlsl"

cbuffer cbGameObjectInfo : register(b0)
{
	float gfSlashWeight : packoffset(c0.x);
	float gfSlideWeight : packoffset(c0.y);
	float gfCrackWeight : packoffset(c0.z);
	float3 gxmf3Color : packoffset(c1);
	float gfUVOffsetX : packoffset(c1.w);
	float gfUVOffsetY : packoffset(c2.x);
	float gfUVScaleX : packoffset(c2.y);
	float gfUVScaleY : packoffset(c2.z);
	float gfEmissiveFactor : packoffset(c2.w);
	float gfGenericAlpha : packoffset(c3.x);
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

Texture2D gtxMultiRenderTargetTextures[7] : register(t35);
SamplerState gSamplerState : register(s0);
SamplerState gSamplerStateClamp : register(s3);
Texture2D gtxtTexture[29] : register(t51);

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

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 viewSpaceDir : TEXCOORD1;
};

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfLifeTime : packoffset(c0.z);
	bool		bEmit : packoffset(c0.w);

	uint2		iTextureCoord : packoffset(c1.x);
	uint		iTextureIndex : packoffset(c1.z);
	uint		gnParticleType : packoffset(c1.w);

	float3		gf3Gravity : packoffset(c2.x);
	float		gfSpeed : packoffset(c2.w);

	float3		gfColor : packoffset(c3.x);
	uint		gnFlareParticlesToEmit : packoffset(c3.w);

	float2		gfSize : packoffset(c4.x);

};

#define MAX_TRAILCONTROLLPOINT 200
#define MAX_COLORCURVES 8
cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints1[MAX_TRAILCONTROLLPOINT];
	float4 gControllpoints2[MAX_TRAILCONTROLLPOINT];
	float4 gfR_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfG_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfB_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfColorCurveTimes[MAX_COLORCURVES / 4];
	uint gnCurves;
	float gfNoiseConstants;
	uint gnPoints;
	float gnOffsetTime;
	float gnEmissiveFactor;
	uint gnBaseTextureIndex;
	uint gnNoiseTextureIndex;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_BreakScreen(VS_SCREEN_RECT_TEXTURED_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float C_PI = 3.141592;
	//float4 cColor = { 1,1,1,1 };
	//int index = 0;
	//float2 texCoord = input.uv * 2.0f - 1.0f;
	//float ColorUV = frac((gfCrackWeight / 7.0f) / 4.0f + gfCurrentTime * 0.2f);
	////float ColorUV = frac(texCoord.x * sin(45 * C_PI / 180.0f) + texCoord.y * cos(45 * C_PI / 180.0f));
	////float ColorUV = frac(sqrt(pow(texCoord.x, 2) + pow(texCoord.y, 2)) + gfCurrentTime);
	//for (int i = 0; i < gnCurves; ++i) {
	//	if (gfColorCurveTimes[i / 4][i % 4] >= ColorUV) {
	//		index = max(0, i - 1);
	//		break;
	//	}
	//}

	//float t = ColorUV - gfColorCurveTimes[index / 4][index % 4];
	//t *= 1.0f / ((float)gfColorCurveTimes[(index + 1) / 4][(index + 1) % 4] - (float)gfColorCurveTimes[index / 4][index % 4]);


	//float4 EmissiveColor;
	//EmissiveColor.r = lerp(gfR_CurvePoints[index / 4][index % 4], gfR_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
	//EmissiveColor.g = lerp(gfG_CurvePoints[index / 4][index % 4], gfG_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
	//EmissiveColor.b = lerp(gfB_CurvePoints[index / 4][index % 4], gfB_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);

	//cColor.rgb *= EmissiveColor.rgb;

	float4 newPos = input.position;
	newPos.y += gfCurrentTime * 1.0f;
	newPos.x += gfCurrentTime * 2.0f;
	float NoiseColor = snoise(float4(newPos.x, newPos.y, newPos.z, gfCurrentTime * 0.5f), 1.0f, 1.0f);
	float NoiseColor2 = snoise(float4(newPos.x, newPos.y + 43, newPos.z, gfCurrentTime * 0.5f), 1.0f, 1.0f);
	//NoiseColor = NoiseColor * 0.5f + 0.5f;

	float4 CrackColor = gtxtTexture[27].Sample(gSamplerState, input.uv * float2(gfUVScaleX, gfUVScaleY) + float2(gfUVOffsetX + NoiseColor * 0.005f, gfUVOffsetY + NoiseColor2 * 0.005f));

	NoiseColor = clamp(NoiseColor, 0.0, 0.8);
	CrackColor = 1.0f - CrackColor;
	//CrackColor.rgb *= gfCrackWeight * cColor.rgb;
	//CrackColor.rgb *= gfCrackWeight * gxmf3Color * gfEmissiveFactor * 0.1f;
	CrackColor.rgb *= gfCrackWeight * gxmf3Color * NoiseColor * gfEmissiveFactor;



	float c = float(FRAME_BUFFER_HEIGHT) * 0.15f;
	float a = 1 / 2.5f;
	float x = input.position.x;
	float y;
	y = a * x + c;

	float4 SlashColor = { 0,0,0,1 };
	float2 MainUV = input.position.xy;

	if (abs(y - (FRAME_BUFFER_HEIGHT - input.position.y)) < gfSlashWeight) {
		//SlashColor.rgb += float3(20.0f, 20.0f, 20.0f) * cColor.rgb;
		SlashColor.rgb += float3(20.0f, 20.0f, 20.0f) * gxmf3Color * gfEmissiveFactor;
	}

	//for (int i = 0; i < 6; ++i) {
	//	float TestSin = sin((x / 200.0) + 4.0f * gfCurrentTime + (i + 1)) * 150.0 * (1.0f - input.uv.x) + (a * x + c);
	//	//float TestSin = sin((x / 10.0) * (C_PI * 2.0f) + 2.0f * gfCurrentTime + (i+1)) * 10.0 + (a * x + c);
	//	//TestSin += ((a * FRAME_BUFFER_WIDTH + c) / FRAME_BUFFER_HEIGHT - 0.2f);
	//	if (abs(TestSin - (FRAME_BUFFER_HEIGHT - input.position.y)) < 2.0f) {
	//		SlashColor.rgb += float3(20.0f, 20.0f, 20.0f) * gxmf3Color * gfEmissiveFactor;
	//	}
	//}

	//else if (abs(y + CalculrateCulrNoise(newPos.xyz).y - (FRAME_BUFFER_HEIGHT - input.position.y)) < gfSlashWeight * 4) {
	//	float4 newPos = input.position;
	//	newPos.y += gfCurrentTime * 2.0f;;
	//	newPos.x += gfCurrentTime * 2.0f;
	//	//newPos.z += gfCurrentTime * 2.0f;
	//	//float NoiseColor = CalculrateCulrNoise(newPos.xyz);
	//	SlashColor.rgb += float3(1.0, 0.2, 0.2) * 20.0f;
	//}


	if (y < (FRAME_BUFFER_HEIGHT - input.position.y)) {
		float2 dir = { 1, a };
		dir = normalize(dir);
		float newX = MainUV.x - dir.x * (gfSlideWeight);
		float newY = (FRAME_BUFFER_HEIGHT - MainUV.y) - dir.y * (gfSlideWeight * (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT));
		MainUV = float2(newX, (FRAME_BUFFER_HEIGHT - newY));
	}
	else {
		float2 dir = { 1, a };
		dir = normalize(dir);
		float newX = MainUV.x + dir.x * (gfSlideWeight);
		float newY = (FRAME_BUFFER_HEIGHT - MainUV.y) + dir.y * (gfSlideWeight * (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT));
		MainUV = float2(newX, (FRAME_BUFFER_HEIGHT - newY));
	}
	MainUV.x /= FRAME_BUFFER_WIDTH;
	MainUV.y /= FRAME_BUFFER_HEIGHT;

	float4 MainColor = gtxMultiRenderTargetTextures[0].Sample(gSamplerStateClamp, MainUV);
	float4 AddColor = { 0,0,0,0 };
	AddColor.rgb = CrackColor.rgb + SlashColor;
	//AddColor.rgb *= gfGenericAlpha;

	//MainColor.rgb *= float3(0.35f, 0.35f, 0.35f);
	MainColor.rgb += AddColor.rgb * gfGenericAlpha;
	MainColor.a = 1.0f;


	output.f4Trail = MainColor;

	return output;
}