cbuffer LevelInfo : register(b0)
{
	float gnLevels : packoffset(c0.x);
	float gnRedution : packoffset(c0.y);
	float2 gnResoultion : packoffset(c0.z);
}

Texture2D gtxtSource : register(t50);
RWTexture2D<float4> gtxtRWOutput : register(u0);
RWTexture2D<float4> gtxtRWFillters[5] : register(u5);
RWTexture2D<float4> gtxtRWBlurs[5] : register(u10);

SamplerState gSamplerState : register(s0);

static float3 gf3ToLuminance = float3(0.3f, 0.59f, 0.11f);
static const float SampleWeights[13] =
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

static float gfGaussianBlurMask2D_2[5][5] = {
	{ 1.0f / 213.0f, 4.0f / 213.0f, 7.0f / 213.0f, 4.0f / 213.0f, 1.0f / 213.0f },
	{ 4.0f / 213.0f, 16.0f / 213.0f, 26.0f / 213.0f, 16.0f / 213.0f, 4.0f / 213.0f },
	{ 7.0f / 213.0f, 26.0f / 213.0f, 41.0f / 213.0f, 26.0f / 213.0f, 7.0f / 213.0f },
	{ 4.0f / 213.0f, 16.0f / 213.0f, 26.0f / 213.0f, 16.0f / 213.0f, 4.0f / 213.0f },
	{ 1.0f / 213.0f, 4.0f / 213.0f, 7.0f / 213.0f, 4.0f / 213.0f, 1.0f / 213.0f }
};

static float gfGaussianBlurMask2D[5][5] = {
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }
};

static float4 TestColorSet[4] = {
	float4(1.0f, 0.0f, 0.0f, 1.0f),
	float4(0.0f, 1.0f, 0.0f, 1.0f),
	float4(0.0f, 0.0f, 1.0f, 1.0f),
	float4(1.0f, 1.0f, 0.0f, 1.0f),
};

#define _WITH_GROUPSHARED_MEMORY

#define RESOULTION_X 1920
#define RESOULTION_Y 1080
//#define REDUTION_SIZE 4

#define DELTA_X					(1.0f / RESOULTION_X)
#define DELTA_Y					(1.0f / RESOULTION_Y)

[numthreads(32, 32, 1)]
void Bloom_CS(uint3 DTid : SV_DispatchThreadID)
{
	/*if (DTid.x >= (int)gnResoultion.x || DTid.y >= (int)gnResoultion.y)
		return;*/
	int gnLevel = (int)gnLevels.x;
	int REDUTION_SIZE = (int)gnRedution;
	int redutionSize = (int)gnRedution;
	float4 FragColor = gtxtSource[DTid.xy];
	float Brightness = dot(FragColor.rgb, gf3ToLuminance);
	float4 BrightColor;

	if (gnLevel == 0) {
		if (Brightness > 2.0)
			BrightColor = FragColor;
		else
			BrightColor = float4(0.0, 0.0, 0.0, 1.0);

		gtxtRWFillters[0][DTid.xy] = BrightColor;
	}
	else if (gnLevel > 0) {
		int level = gnLevel;
		int sampleSize = redutionSize;

		int2 TexCoord = int2(DTid.x, DTid.y);
		int2 hLevelTexCoord = int2(DTid.x * sampleSize, DTid.y * sampleSize);
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = -sampleSize / 2; i < sampleSize / 2; i++)
		{
			for (int j = -sampleSize / 2; j < sampleSize / 2; j++)
			{
				f4Color += gtxtRWFillters[level - 1][hLevelTexCoord.xy + int2(i, j)];
			}
		}

		gtxtRWFillters[level][TexCoord] = float4((f4Color / ((sampleSize + 1) * (sampleSize + 1))).rgb, 0.0f);
	}


	//for(int level = 1; level < 4 + 1; ++level) {
	//	int sampleSize = redutionSize;

	//	if (!(DTid.x % pow(redutionSize, level) == 0 || DTid.y % pow(redutionSize, level) == 0))
	//		break;

	//	int2 Resoultion = int2(ceil((float)RESOULTION_X / pow(redutionSize, level)), ceil((float)RESOULTION_Y / pow(redutionSize, level)));

	//	int2 hLevelTexCoord = int2(DTid.x / pow(redutionSize, level - 1), DTid.y / pow(redutionSize, level - 1));
	//	int2 TexCoord = int2(DTid.x / pow(redutionSize, level), DTid.y / pow(redutionSize, level));
	//	float4 f4Color = float4(0, 0, 0, 0);
	//	for (int i = -sampleSize/2; i < sampleSize/2; i++)
	//	{
	//		for (int j = -sampleSize/2; j < sampleSize/2; j++)
	//		{
	//			f4Color += gtxtRWFillters[level - 1][hLevelTexCoord.xy + int2(i, j)];
	//		}
	//	}

	//	gtxtRWFillters[level][TexCoord] = float4((f4Color / ((sampleSize+1) * (sampleSize+1))).rgb, 0.0f);
	//	//gtxtRWFillters[level][TexCoord] = gtxtRWFillters[level-1][hLevelTexCoord.xy];
	//	//gtxtRWFillters[level][TexCoord] = (f4Color / (sampleSize * sampleSize)) * (1.0f / level);
	//}

	//GroupMemoryBarrierWithGroupSync();

	//float BloomFactor = 0.18f;
	////float BloomFactor = 0.08f;

	//float4 Color = FragColor;
	//float4 BloomColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//float2 uv = float2(DTid.x / RESOULTION_X, DTid.y / RESOULTION_Y);
	//
	//gtxtRWOutput[DTid.xy] = Color;
}