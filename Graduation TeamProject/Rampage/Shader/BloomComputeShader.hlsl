
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
#define REDUTION_SIZE 4

#define DELTA_X					(1.0f / RESOULTION_X)
#define DELTA_Y					(1.0f / RESOULTION_Y)

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
		int sampleSize = REDUTION_SIZE;

		if (!(DTid.x % pow(REDUTION_SIZE, level) == 0 || DTid.y % pow(REDUTION_SIZE, level) == 0))
			break;

		int2 Resoultion = int2(ceil((float)RESOULTION_X / pow(REDUTION_SIZE, level)), ceil((float)RESOULTION_Y / pow(REDUTION_SIZE, level)));

		int2 hLevelTexCoord = int2(DTid.x / pow(REDUTION_SIZE, level - 1), DTid.y / pow(REDUTION_SIZE, level - 1));
		int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = -1; i < sampleSize - 1; i++)
		{
			for (int j = -1; j < sampleSize - 1; j++)
			{
				f4Color += gtxtRWFillters[level - 1][hLevelTexCoord.xy + int2(i, j)];
			}
		}

		gtxtRWFillters[level][TexCoord] = f4Color / (sampleSize * sampleSize);
	}
	
	GroupMemoryBarrierWithGroupSync();

	float BloomFactor = 0.18f;
	//float BloomFactor = 0.08f;

	float4 Color = FragColor;
	float4 BloomColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float2 uv = float2(DTid.x / RESOULTION_X, DTid.y / RESOULTION_Y);
	for (int level = 1; level < 4 + 1; ++level) {

		/*float4 f4Color = float4(0, 0, 0, 0);

		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{

				f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[level].Load(int2(uv + float2(DELTA_X * i, DELTA_Y * j)));
			}
		}
		Color += f4Color;*/

		int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		float2 DeltaUV = float2(1.0f / (RESOULTION_X / pow(REDUTION_SIZE, level)), 1.0f / (RESOULTION_Y / pow(REDUTION_SIZE, level)));
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
				f4Color += gfGaussianBlurMask2D_2[i+2][j+2] * gtxtRWFillters[level][TexCoord.xy + int2(i, j)];
				
			}
		}
		/*if (level > 2) {
			Color += 0.06f * f4Color;
		}
		else if (level > 3) {
			Color += 0.03f * f4Color;
		}
		else*/
			Color += /*BloomFactor * */f4Color;
		//BloomFactor -= 0.01f;
			gtxtRWBlurs[level][TexCoord] = f4Color;
	}
	//gtxtRWOutput[DTid.xy] =lerp(Color, BloomColor, 0.5f);
	//gtxtRWOutput[DTid.xy] = BrightColor.r > 0.0f ? lerp(Color, BloomColor, 0.7f) : Color;
	gtxtRWOutput[DTid.xy] = Color;
}