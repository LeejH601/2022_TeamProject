
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

static float gfGaussianBlurMask2Ds[3][5][5] = {
	{{ 2.0f / 273.0f, 8.0f / 273.0f, 14.0f / 273.0f, 8.0f / 273.0f, 2.0f / 273.0f },
	{ 8.0f / 273.0f, 30.0f / 273.0f, 41.0f / 273.0f, 30.0f / 273.0f, 8.0f / 273.0f },
	{ 14.0f / 273.0f, 41.0f / 273.0f, 62.0f / 273.0f, 41.0f / 273.0f, 14.0f / 273.0f },
	{ 8.0f / 273.0f, 30.0f / 273.0f, 41.0f / 273.0f, 30.0f / 273.0f, 8.0f / 273.0f },
	{ 2.0f / 273.0f, 8.0f / 273.0f, 14.0f / 273.0f, 8.0f / 273.0f, 2.0f / 273.0f }},

	{{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }},

	{{ 1.0f / 273.0f, 3.0f / 273.0f, 3.0f / 273.0f, 3.0f / 273.0f, 1.0f / 273.0f },
	{ 3.0f / 273.0f, 8.0f / 273.0f, 12.0f / 273.0f, 8.0f / 273.0f, 3.0f / 273.0f },
	{ 3.0f / 273.0f, 12.0f / 273.0f, 26.0f / 273.0f, 12.0f / 273.0f, 3.0f / 273.0f },
	{ 3.0f / 273.0f, 8.0f / 273.0f, 12.0f / 273.0f, 8.0f / 273.0f, 3.0f / 273.0f },
	{ 1.0f / 273.0f, 3.0f / 273.0f, 3.0f / 273.0f, 3.0f / 273.0f, 1.0f / 273.0f }},
};

static float gfGaussianBlurMask2D[5][5] = {
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }
};

static float gfGaussianBlurMask2D_77[7][7] = {
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 10.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f  },
	{ 4.0f / 273.0f, 10.0f / 273.0f,16.0f / 273.0f,	26.0f / 273.0f,	16.0f / 273.0f,	10.0f / 273.0f,  4.0f / 273.0f  },
	{ 7.0f / 273.0f, 16.0f / 273.0f,26.0f / 273.0f,	41.0f / 273.0f,	26.0f / 273.0f,	16.0f / 273.0f,  7.0f / 273.0f  },
	{ 10.0f / 273.0f,26.0f / 273.0f,41.0f / 273.0f, 52.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 10.0f / 273.0f },
	{ 7.0f / 273.0f, 16.0f / 273.0f,26.0f / 273.0f,	41.0f / 273.0f,	26.0f / 273.0f,	16.0f / 273.0f,  7.0f / 273.0f  },
	{ 4.0f / 273.0f, 10.0f / 273.0f,16.0f / 273.0f,	26.0f / 273.0f,	16.0f / 273.0f,	10.0f / 273.0f,  4.0f / 273.0f  },
	{  1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 10.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f  }
};



#define BICUBIC

//[numthreads(32, 32, 1)]
[numthreads(1, 1, 1)]
void BloomApply_CS(uint3 DTid : SV_DispatchThreadID)
{
	int gnLevel = (int)gnLevels.x;
	
	//float4 BloomColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	int2 TexCoord = DTid.xy;
	float4 f4Color = float4(0, 0, 0, 0);

	if (gnLevel > 2) {
		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				//f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];
				f4Color += gfGaussianBlurMask2Ds[1][i + 2][j + 2] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];

			}
		}
		/*for (int i = -3; i <= 3; i++)
		{
			for (int j = -3; j <= 3; j++)
			{
				f4Color += gfGaussianBlurMask2D_77[i + 3][j + 3] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];

			}
		}*/

		gtxtRWBlurs[gnLevel][TexCoord] = float4(f4Color.rgb, 0.0f);
	}

	else if (gnLevel > 0) {

		float2 Weigth_plus = float2(TexCoord / REDUTION_SIZE);
		int2 TexCoord_plus = floor(Weigth_plus);
		Weigth_plus = frac(Weigth_plus);

		float plusColor;
		//gtxtRWBlurs[level][TexCoord] = gtxtRWFillters[level][TexCoord];
		
#ifdef BICUBIC
		{
			float4 plusColor1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, -1)];
			float4 plusColor2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, -1)];
			float4 plusColor5 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, 0)];
			float4 plusColor6 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 0)];

			float4 plusColor3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, -1)];
			float4 plusColor4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(2, 1)];
			float4 plusColor7 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];
			float4 plusColor8 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(2, 1)];

			float4 plusColor9 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, 1)];
			float4 plusColor10 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
			float4 plusColor13 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(-1, 2)];
			float4 plusColor14 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 2)];

			float4 plusColor11 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];
			float4 plusColor12 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(2, 1)];
			float4 plusColor15 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 2)];
			float4 plusColor16 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(2, 2)];

			//
			float4 pluslerp12 = lerp(plusColor1, plusColor2, Weigth_plus.x);
			float4 pluslerp56 = lerp(plusColor5, plusColor6, Weigth_plus.x);

			float4 pluslerp23 = lerp(plusColor2, plusColor3, Weigth_plus.x);
			float4 pluslerp67 = lerp(plusColor6, plusColor7, Weigth_plus.x);

			float4 pluslerp34 = lerp(plusColor3, plusColor4, Weigth_plus.x);
			float4 pluslerp78 = lerp(plusColor7, plusColor8, Weigth_plus.x);

			float4 pluslerp910 = lerp(plusColor9, plusColor10, Weigth_plus.x);
			float4 pluslerp1314 = lerp(plusColor13, plusColor14, Weigth_plus.x);

			float4 pluslerp1011 = lerp(plusColor10, plusColor11, Weigth_plus.x);
			float4 pluslerp1415 = lerp(plusColor14, plusColor15, Weigth_plus.x);

			float4 pluslerp1112 = lerp(plusColor11, plusColor12, Weigth_plus.x);
			float4 pluslerp1516 = lerp(plusColor15, plusColor16, Weigth_plus.x);

			//
			float4 LT = lerp(pluslerp12, pluslerp56, Weigth_plus.y);
			float4 TC = lerp(pluslerp23, pluslerp67, Weigth_plus.y);
			float4 RT = lerp(pluslerp34, pluslerp78, Weigth_plus.y);

			float4 L = lerp(pluslerp56, pluslerp910, Weigth_plus.y);
			float4 C = lerp(pluslerp67, pluslerp1011, Weigth_plus.y);
			float4 R = lerp(pluslerp78, pluslerp1112, Weigth_plus.y);

			float4 LB = lerp(pluslerp910, pluslerp1314, Weigth_plus.y);
			float4 BC = lerp(pluslerp1011, pluslerp1415, Weigth_plus.y);
			float4 RB = lerp(pluslerp1112, pluslerp1516, Weigth_plus.y);

			//
			float LTTC = lerp(LT, TC, Weigth_plus.x);
			float TCRT = lerp(TC, RT, Weigth_plus.x);

			float LC = lerp(L, C, Weigth_plus.x);
			float CR = lerp(C, R, Weigth_plus.x);

			float LBBC = lerp(LB, BC, Weigth_plus.x);
			float RCRB = lerp(BC, RB, Weigth_plus.x);

			//
			float4 S1 = lerp(LTTC, LC, Weigth_plus.y);
			float4 S2 = lerp(TCRT, CR, Weigth_plus.y);
			float4 S3 = lerp(LC, LBBC, Weigth_plus.y);
			float4 S4 = lerp(CR, RCRB, Weigth_plus.y);

			//
			float4 S1S2 = lerp(S1, S2, Weigth_plus.x);
			float4 S3S4 = lerp(S3, S4, Weigth_plus.x);

			//
			plusColor = lerp(S1S2, S3S4, Weigth_plus.y);
		}
#else
		{
			float4 plusColor1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
			float4 plusColor2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];
			float4 plusColor3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
			float4 plusColor4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];
			float4 pluslerp1 = lerp(plusColor1, plusColor2, Weigth_plus.x);
			float4 pluslerp2 = lerp(plusColor3, plusColor4, Weigth_plus.x);
			plusColor = lerp(pluslerp1, pluslerp2, Weigth_plus.y);
		}
#endif
		

		//plusColor =  gtxtRWBlurs[gnLevel + 1][TexCoord_plus];

		//gtxtRWFillters[gnLevel][TexCoord] += float4(lerp(gtxtRWFillters[gnLevel][TexCoord], plusColor, 0.5f).rgb, 0.0f);
		//gtxtRWFillters[gnLevel][TexCoord] += plusColor;
		//gtxtRWFillters[gnLevel][TexCoord] = plusColor;
		//gtxtRWFillters[gnLevel][TexCoord] += 0.4f *  gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		//gtxtRWFillters[gnLevel][TexCoord] += gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		//gtxtRWFillters[gnLevel][TexCoord] += max(gtxtRWFillters[gnLevel][TexCoord],  gtxtRWBlurs[gnLevel + 1][TexCoord_plus]);
		gtxtRWFillters[gnLevel][TexCoord] += max(gtxtRWFillters[gnLevel][TexCoord], plusColor);

		GroupMemoryBarrierWithGroupSync();

		f4Color = float4(0, 0, 0, 0);

		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
				//f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];
				f4Color += gfGaussianBlurMask2Ds[1][i + 2][j + 2] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];
			}
		}
		/*for (int i = -3; i <= 3; i++)
		{
			for (int j = -3; j <= 3; j++)
			{
				f4Color += gfGaussianBlurMask2D_77[i + 3][j + 3] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];

			}
		}*/
		gtxtRWBlurs[gnLevel][TexCoord] = float4(f4Color.rgb,0.0f);
		//gtxtRWBlurs[gnLevel][TexCoord] += gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		//gtxtRWBlurs[gnLevel][TexCoord] += plusColor;


	}
}