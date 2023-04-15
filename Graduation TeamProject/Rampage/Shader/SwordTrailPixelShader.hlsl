#include "Header.hlsli"

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
};

static float CurveTimes[4] = { 0, 0.3, 0.6, 1.0 };
static float R_CurvePoints[4] = { 0, 0.14, 0.459, 1.892 };
static float G_CurvePoints[4] = { 0, 0.005, 0.067, 0.595 };
static float B_CurvePoints[4] = { 0, 0.257, 0.26, 0 };

PS_MULTIPLE_RENDER_TARGETS_OUTPUT SwordTrail_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float2 uv = input.uv.xy;
	//uv.y /= 2.0f;
	float4 BaseColor = gtxMappedTexture[0].Sample(gSamplerState, uv * float2(1.0f, 0.25f));
	float4 NoiseColor = gtxMappedTexture[1].Sample(gSamplerState, uv * float2(1.0f, 4.0f));
	//float4 fColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);

	float EmissiveFactor = min(1.0f, uv.x * 4.0f);
	float4 EmissiveColor;
	for (int i = 0; i < 4 - 1; ++i) {
		if (CurveTimes[i] <= uv.x && CurveTimes[i+1] > uv.x) {
			float t = uv.x - CurveTimes[i];
			t *= 1.0f / (CurveTimes[i + 1] - CurveTimes[i]);

			EmissiveColor.r = lerp(R_CurvePoints[i], R_CurvePoints[i + 1], t);
			EmissiveColor.g = lerp(G_CurvePoints[i], G_CurvePoints[i + 1], t);
			EmissiveColor.b = lerp(B_CurvePoints[i], B_CurvePoints[i + 1], t);
			EmissiveColor.a = 1.0f;

			break;
		}
	}
	EmissiveColor.rgb *= EmissiveFactor * 60.0f ;

	float4 fColor;
	fColor = BaseColor * NoiseColor * EmissiveColor;
	//fColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
	//fColor.rgb *= 2.0f;
	//fColor.rgb *= NoiseColor.rgb;

	output.f4Trail = fColor;
	output.f4Color = fColor;

	return output;
}