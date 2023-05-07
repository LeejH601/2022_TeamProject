#include "Header.hlsli"

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

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
}

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
};

//static float CurveTimes[4] = { 0.0f, 0.3, 0.6, 1.0 };
//static float R_CurvePoints[4] = { 0.0f, 0.14, 0.459, 1.892 };
//static float G_CurvePoints[4] = { 0.0f, 0.005, 0.067, 0.595 };
//static float B_CurvePoints[4] = { 0.0f, 0.257, 0.26, 0.0f };

PS_MULTIPLE_RENDER_TARGETS_OUTPUT SwordTrail_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float2 uv = input.uv.xy;
	//uv.y /= 2.0f;
	float4 BaseColor = gtxMappedTexture[0].Sample(gSamplerState, uv * float2(1.0f, 1.0f));
	float4 NoiseColor = gtxMappedTexture[1].Sample(gSamplerState, uv * float2(1.0f, 1.0f) - gnOffsetTime);

	float EmissiveFactor = 1.5f;
	//float EmissiveFactor = min(2.0f, uv.x * 4.0f);
	float4 EmissiveColor;
	int index = 0;

	float uv1 = abs(uv.x * 2.0f - 1.0f);
	float uv2 = 1.0f - uv1;

	//float ColorUV = lerp(uv1, uv2, frac(abs(fmod(gnOffsetTime * 0.25f + uv.x, 2.0f) - 1.0f)));
	// 
	// uv를 이용한 색상
	// float ColorUV = 1.0f - uv.y;
	// Noise를 이용한 색상
	float ColorUV = min(1.0f, NoiseColor.r * gfNoiseConstants);

	//float ColorUV = 1.0 - abs(uv.x * 2.0f - 1.0f);
	//float ColorUV = frac(abs(fmod(gnOffsetTime + uv.x, 2.0f) - 1.0f));
	//float ColorUV = frac(uv.x + gnOffsetTime);


	for (int i = 0; i < gnCurves; ++i) {
		if (gfColorCurveTimes[i / 4][i % 4] >= ColorUV) {
			index = max(0, i - 1);
			break;
		}
	}


	float t = ColorUV - gfColorCurveTimes[index / 4][index % 4];
	t *= 1.0f / ((float)gfColorCurveTimes[(index + 1) / 4][(index + 1) % 4] - (float)gfColorCurveTimes[index / 4][index % 4]);
	//t = ((1.0f - BaseColor.r) * 1.f ) + 0.4f;
	//t -= BaseColor.r;
	//t = saturate(t);

	EmissiveColor.r = lerp(gfR_CurvePoints[index / 4][index % 4], gfR_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
	EmissiveColor.g = lerp(gfG_CurvePoints[index / 4][index % 4], gfG_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
	EmissiveColor.b = lerp(gfB_CurvePoints[index / 4][index % 4], gfB_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);

	EmissiveColor.a = uv.x;
	//EmissiveColor.rgb = float3(t, 0.0f, 0.0f);

	EmissiveColor.rgb *= EmissiveFactor * 20.0f * NoiseColor.r;

	float4 fColor;
	fColor = BaseColor * EmissiveColor;
	//fColor.a = 0.5f;
	//fColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
	//fColor.rgb *= 2.0f;
	//fColor.rgb *= NoiseColor.rgb;
	//fColor.r = max(0.0f, fColor.r);
	//fColor.g = max(0.0f, fColor.g);
	//fColor.b = max(0.0f, fColor.b);
	//fColor.a = max(0.0f, fColor.a);

	output.f4Trail = fColor;
	output.f4Color = fColor;

	return output;
}