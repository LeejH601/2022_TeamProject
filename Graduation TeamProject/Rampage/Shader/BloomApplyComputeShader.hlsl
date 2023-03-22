
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

[numthreads(32, 32, 1)]
void BloomApply_CS(uint3 DTid : SV_DispatchThreadID)
{
	float BloomFactor = 1.0f;
	float4 Color = gtxtSource[DTid.xy];
	//float4 BloomColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float2 uv = float2(DTid.x / RESOULTION_X, DTid.y / RESOULTION_Y);
	float2 uvWeight = float2(DTid.x % RESOULTION_X, DTid.y % RESOULTION_Y);
	float4 BloomColor = gtxtRWBlurs[3][int2(DTid.x / pow(REDUTION_SIZE, 1), DTid.y / pow(REDUTION_SIZE, 1))];

	int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, 3), DTid.y / pow(REDUTION_SIZE, 3));
	float4 f4Color = float4(0, 0, 0, 0);
	{
		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[3][TexCoord.xy + int2(i, j)];

			}
		}
		gtxtRWBlurs[3][TexCoord] = f4Color;
	}

	GroupMemoryBarrierWithGroupSync();

	for (int level = 2; level > 0; --level) {

		float2 Weigth = float2(DTid.x % pow(REDUTION_SIZE, level), DTid.y % pow(REDUTION_SIZE, level));
		float2 DeltaUV = float2(1.0f / (RESOULTION_X / pow(REDUTION_SIZE, level)), 1.0f / (RESOULTION_Y / pow(REDUTION_SIZE, level)));
		float2 Delta = float2(1.0f / RESOULTION_X, 1.0f / RESOULTION_Y);
		TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		int2 TexCoord_plus = int2(DTid.x / pow(REDUTION_SIZE, level+1), DTid.y / pow(REDUTION_SIZE, level+1));
		
		//gtxtRWBlurs[level][TexCoord] = gtxtRWFillters[level][TexCoord];

		gtxtRWFillters[level][TexCoord] = lerp(gtxtRWFillters[level][TexCoord], gtxtRWBlurs[level + 1][TexCoord_plus], 0.5f);

		{
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
					f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[level][TexCoord.xy + int2(i, j)];

				}
			}
			gtxtRWBlurs[level][TexCoord] = f4Color;
		}

		GroupMemoryBarrierWithGroupSync();



		// 5°³ ÇÈ¼¿ Æò±Õ
		/*f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-2, -2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-1, -2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0, -2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1, -2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(2, -2)];

		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-2, -1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-1, -1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0, -1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1, -1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(2, -1)];

		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-2, 0)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-1, 0)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0, 0)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1, 0)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(2, 0)];

		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-2, 1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-1, 1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0, 1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1, 1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(2, 1)];

		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-2, 2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(-1, 2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0, 2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1, 2)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(2, 2)];
		
		f4Color /= 25;*/

	}
	gtxtRWBlurs[0][DTid.xy] = BloomColor;

	GroupMemoryBarrierWithGroupSync();

	float4 FinalBloomColor;

	
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1,0)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 1)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 1)];
	//FinalBloomColor /= 4;




	gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
	//gtxtRWOutput[DTid.xy] = float4(lerp(Color.xyz, BloomColor.xyz, 0.5f), Color.w);

}