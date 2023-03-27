cbuffer LevelInfo : register(b0)
{
	float4 gnLevels : packoffset(c0);
}

Texture2D gtxtSource : register(t50);
RWTexture2D<float4> gtxtRWOutput : register(u0);
RWTexture2D<float4> gtxtRWFillters[5] : register(u5);
RWTexture2D<float4> gtxtRWBlurs[5] : register(u10);
//Texture2D<float4> gtxtRWBlurs_srv[5] : register(t55);

SamplerState gSamplerState : register(s0);

#define RESOULTION_X 1920
#define RESOULTION_Y 1080
#define REDUTION_SIZE 4


#define binary

[numthreads(30, 16, 1)]
void BloomUpsample_CS(uint3 DTid : SV_DispatchThreadID)
{
	int gnLevel = (int)gnLevels.x;
	gnLevel -= 1;
	int2 TexCoord = DTid.xy;
	float2 Weigth_plus = float2(TexCoord / REDUTION_SIZE);
	int2 TexCoord_plus = floor(Weigth_plus);
	Weigth_plus = frac(Weigth_plus);
#ifdef binary
	float4 plusColor;
	{
		float4 plusColor1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		float4 plusColor2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];
		float4 plusColor3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
		float4 plusColor4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];
		float4 pluslerp1 = lerp(plusColor1, plusColor2, Weigth_plus.x);
		float4 pluslerp2 = lerp(plusColor3, plusColor4, Weigth_plus.x);
		plusColor = lerp(pluslerp1, pluslerp2, Weigth_plus.y);
	}
#else
	{
		float4 Sample1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, -1)];
		float4 Sample2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, -1)];
		float4 Sample3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, -1)];

		float4 Sample4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, 0)];
		float4 Sample5 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		float4 Sample6 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];

		float4 Sample7 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, 1)];
		float4 Sample8 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
		float4 Sample9 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];

		plusColor = Sample1 + Sample2 + Sample3 + Sample4 + Sample5 + Sample6 + Sample7 + Sample8 + Sample9;
		plusColor /= 9;
		plusColor.w = 0.0f;

	/*	float4 plusColor1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		float4 plusColor2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];
		float4 plusColor3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
		float4 plusColor4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];
		float4 pluslerp1 = lerp(plusColor1, plusColor2, Weigth_plus.x);
		float4 pluslerp2 = lerp(plusColor3, plusColor4, Weigth_plus.x);
		float4 plusColor = lerp(pluslerp1, pluslerp2, Weigth_plus.y);*/
	}
#endif

	gtxtRWFillters[gnLevel][TexCoord] += max(gtxtRWFillters[gnLevel][TexCoord], plusColor);
	//gtxtRWFillters[gnLevel][TexCoord] += lerp(gtxtRWFillters[gnLevel][TexCoord], gtxtRWBlurs[gnLevel + 1][TexCoord_plus], 0.5f);
	//gtxtRWFillters[gnLevel][TexCoord] += max(gtxtRWFillters[gnLevel][TexCoord], gtxtRWBlurs[gnLevel + 1][TexCoord_plus]);
}