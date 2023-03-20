
Texture2D gtxtSource : register(t50);
RWTexture2D<float4> gtxtRWOutput : register(u0);
RWTexture2D<float4> gtxtRWFillters[5] : register(u5);

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
#define REDUTION_SIZE 2

[numthreads(32, 32, 1)]
void Bloom_CS( uint3 DTid : SV_DispatchThreadID )
{
	float4 FragColor = gtxtSource[DTid.xy];
	float Brightness = dot(FragColor.rgb, gf3ToLuminance);
	float4 BrightColor;

	if (Brightness > 1.5)
		BrightColor = FragColor;
	else
		BrightColor = float4(0.0, 0.0, 0.0, 1.0);

	gtxtRWFillters[0][DTid.xy] = BrightColor;

	GroupMemoryBarrierWithGroupSync();


	for(int level = 1; level < 4 + 1; ++level) {
		if (!(DTid.x % pow(REDUTION_SIZE, level) || DTid.y % pow(REDUTION_SIZE, level)))
			break;

		int2 Resoultion = int2(ceil((float)RESOULTION_X / pow(REDUTION_SIZE, level)), ceil((float)RESOULTION_Y / pow(REDUTION_SIZE, level)));

		int2 hLevelTexCoord = int2(DTid.x / pow(REDUTION_SIZE, level - 1), DTid.y / pow(REDUTION_SIZE, level - 1));
		int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				f4Color += gtxtRWFillters[level - 1][hLevelTexCoord.xy + int2(i, j)];
			}
		}

		gtxtRWFillters[level][TexCoord] = f4Color / 16;
	}
	
	GroupMemoryBarrierWithGroupSync();

	float BloomFactor = 0.2f;

	float4 Color = FragColor;
	for (int level = 1; level < 4 + 1; ++level) {
		int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][TexCoord.xy + int2(i, j)];
			}
		}
		Color += BloomFactor * f4Color;
		BloomFactor -= 0.045f;
	}
	gtxtRWOutput[DTid.xy] = Color;
}