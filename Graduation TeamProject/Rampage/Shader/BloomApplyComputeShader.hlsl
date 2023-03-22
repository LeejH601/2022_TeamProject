
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

static float gfGaussianBlurMask2D[5][5] = {
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }
};

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
				f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[3][TexCoord + int2(i, j)];

			}
		}
		gtxtRWBlurs[3][TexCoord] = f4Color;
	}
	else if (gnLevel > 0) {

		float2 Weigth_plus = float2(TexCoord / REDUTION_SIZE);
		int2 TexCoord_plus = floor(Weigth_plus);
		Weigth_plus = frac(Weigth_plus);

		float plusColor;
		//gtxtRWBlurs[level][TexCoord] = gtxtRWFillters[level][TexCoord];
		float4 plusColor1 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		float4 plusColor2 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 0)];
		float4 plusColor3 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(0, 1)];
		float4 plusColor4 = gtxtRWBlurs[gnLevel + 1][TexCoord_plus + int2(1, 1)];
		float4 pluslerp1 = lerp(plusColor1, plusColor2, Weigth_plus.x);
		float4 pluslerp2 = lerp(plusColor3, plusColor4, Weigth_plus.x);
		plusColor = lerp(pluslerp1, pluslerp2, Weigth_plus.y);

		//plusColor =  gtxtRWBlurs[gnLevel + 1][TexCoord_plus];

		//gtxtRWFillters[gnLevel][TexCoord] += lerp(gtxtRWFillters[gnLevel][TexCoord], plusColor, 0.5f);
		gtxtRWFillters[gnLevel][TexCoord] += gtxtRWBlurs[gnLevel + 1][TexCoord_plus];
		//gtxtRWFillters[gnLevel][TexCoord] += 0.2f *  gtxtRWBlurs[gnLevel + 1][TexCoord_plus];

		GroupMemoryBarrierWithGroupSync();

		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
				f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[gnLevel][TexCoord + int2(i, j)];

			}
		}
		gtxtRWBlurs[gnLevel][TexCoord] = f4Color;
	}


	if (gnLevel == 0) {
		float BloomFactor = 0.5f;
		float4 Color = gtxtSource[DTid.xy];
		int2 uv = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
		float2 uvWeight = frac(DTid / REDUTION_SIZE);

		/*float4 Color1 = gtxtRWBlurs[1][uv];
		float4 Color2 = gtxtRWBlurs[1][uv + int2(1, 0)];
		float4 Color3 = gtxtRWBlurs[1][uv + int2(0, 1)];
		float4 Color4 = gtxtRWBlurs[1][uv + int2(1, 1)];
		float4 lerp1 = lerp(Color1, Color2, uvWeight.x);
		float4 lerp2 = lerp(Color3, Color4, uvWeight.x);
		float4 BloomColor = lerp(lerp1, lerp2, uvWeight.y);
		gtxtRWBlurs[0][DTid.xy] = BloomColor;


		gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);*/

		float4 BloomColor = gtxtRWBlurs[1][uv];
		gtxtRWBlurs[0][DTid.xy] = BloomColor;


		gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
	}

	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1,0)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 1)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 1)];
	//FinalBloomColor /= 4;

	/*int2 uv = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
	float2 uvWeight = frac(DTid / REDUTION_SIZE);

	float4 Color1 = gtxtRWBlurs[1][uv];
	float4 Color2 = gtxtRWBlurs[1][uv + int2(1, 0)];
	float4 Color3 = gtxtRWBlurs[1][uv + int2(0, 1)];
	float4 Color4 = gtxtRWBlurs[1][uv + int2(1, 1)];
	float4 lerp1 = lerp(Color1, Color2, uvWeight.x);
	float4 lerp2 = lerp(Color3, Color4, uvWeight.x);
	BloomColor = lerp(lerp1, lerp2, uvWeight.y);
	gtxtRWBlurs[0][DTid.xy] = BloomColor;*/

	//gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
	//gtxtRWOutput[DTid.xy] = float4(lerp(Color.xyz, BloomColor.xyz, 0.5f), Color.w);

}