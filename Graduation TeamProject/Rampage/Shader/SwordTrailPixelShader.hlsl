#include "Header.hlsli"

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

#define MAX_TRAILCONTROLLPOINT 100
cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints1[MAX_TRAILCONTROLLPOINT];
	float4 gControllpoints2[MAX_TRAILCONTROLLPOINT];
	uint gnPoints;
	float gnOffsetTime;
}

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
};

static float CurveTimes[4] = { 0.0f, 0.3, 0.6, 1.0 };
static float R_CurvePoints[4] = { 0.0f, 0.14, 0.459, 1.892 };
static float G_CurvePoints[4] = { 0.0f, 0.005, 0.067, 0.595 };
static float B_CurvePoints[4] = { 0.0f, 0.257, 0.26, 0.0f };

PS_MULTIPLE_RENDER_TARGETS_OUTPUT SwordTrail_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float2 uv = input.uv.xy;
	//uv.y /= 2.0f;
	float4 BaseColor = gtxMappedTexture[0].Sample(gSamplerState, uv * float2(1.0f, 1.0f));
	float4 NoiseColor = gtxMappedTexture[1].Sample(gSamplerState, uv * float2(0.2f, 0.2f));
	//float4 NoiseColor = gtxMappedTexture[1].Sample(gSamplerState, uv * float2(32.0f, 32.0f));
	//float4 fColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);

	//float EmissiveFactor = 1.5f;
	float EmissiveFactor = min(2.0f, uv.x * 4.0f);
	float4 EmissiveColor;
	int index = 0;

	float uv1 = abs(uv.x * 2.0f - 1.0f);
	float uv2 = 1.0f - uv1;

	//float ColorUV = lerp(uv1, uv2, frac(abs(fmod(gnOffsetTime * 0.25f + uv.x, 2.0f) - 1.0f)));
	float ColorUV = 1.0f - uv.y;
	//float ColorUV = 1.0 - abs(uv.x * 2.0f - 1.0f);
	//float ColorUV = frac(abs(fmod(gnOffsetTime + uv.x, 2.0f) - 1.0f));
	//float ColorUV = frac(uv.x + gnOffsetTime);
	if (ColorUV >= 0.0f && ColorUV < 0.3f) {
		index = 0;
	}
	if (ColorUV >= 0.3f && ColorUV < 0.6f) {
		index = 1;
	}
	if (ColorUV >= 0.6f && ColorUV <= 1.0f) {
		index = 2;
	}

	/*for (int i = 0; i < 4; ++i) {
		if (CurveTimes[i] < uv.x) {
			continue;
		}
		else {
			index = i - 1;
			break;
		}
	}
	index = max(0, index);*/
	/*if (index >= 3)
		index--;*/

	float t = ColorUV - CurveTimes[index];
	t *= 1.0f / ((float)CurveTimes[index + 1] - (float)CurveTimes[index]);
	//t = ((1.0f - BaseColor.r) * 1.f ) + 0.4f;
	//t -= BaseColor.r;
	//t = saturate(t);

	EmissiveColor.r = lerp(R_CurvePoints[index], R_CurvePoints[index + 1], t);
	EmissiveColor.g = lerp(G_CurvePoints[index], G_CurvePoints[index + 1], t);
	EmissiveColor.b = lerp(B_CurvePoints[index], B_CurvePoints[index + 1], t);

	//EmissiveColor.r = lerp(R_CurvePoints[index + 1], R_CurvePoints[index], t);
	//EmissiveColor.g = lerp(G_CurvePoints[index + 1], G_CurvePoints[index], t);
	//EmissiveColor.b = lerp(B_CurvePoints[index + 1], B_CurvePoints[index], t);
	EmissiveColor.a = uv.x;
	//EmissiveColor.rgb = float3(t, 0.0f, 0.0f);

	EmissiveColor.rgb *= EmissiveFactor * 20.0f;

	float4 fColor;
	fColor = BaseColor * NoiseColor * EmissiveColor;
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