
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
	float4 BloomColor = gtxtRWBlurs[3][int2(DTid.x / pow(REDUTION_SIZE, 1), DTid.y / pow(REDUTION_SIZE, 1))];

	int2 TexCoord;
	float2 TexBuffer = float2(DTid.x / pow(REDUTION_SIZE, 3), DTid.y / pow(REDUTION_SIZE, 3));
	float4 f4Color = float4(0, 0, 0, 0);
	if (frac(TexBuffer.x) < 0.000001f && frac(TexBuffer.y) < 0.000001f) {
		TexCoord = float2(floor(TexBuffer.x), floor(TexBuffer.y));
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

		TexBuffer = DTid / pow(REDUTION_SIZE, level);
		TexCoord = floor(TexBuffer);

		float2 Weigth_plus =TexBuffer / REDUTION_SIZE;
		int2 TexCoord_plus = floor(Weigth_plus);
		Weigth_plus = frac(Weigth_plus);

		float plusColor;
		//gtxtRWBlurs[level][TexCoord] = gtxtRWFillters[level][TexCoord];
		if (frac(TexBuffer.x) < 0.000001f && frac(TexBuffer.y) < 0.000001f) {
			float4 plusColor1 = gtxtRWBlurs[level + 1][TexCoord_plus];
			float4 plusColor2 = gtxtRWBlurs[level + 1][TexCoord_plus + int2(1, 0)];
			float4 plusColor3 = gtxtRWBlurs[level + 1][TexCoord_plus + int2(0, 1)];
			float4 plusColor4 = gtxtRWBlurs[level + 1][TexCoord_plus + int2(1, 1)];
			float4 pluslerp1 = lerp(plusColor1, plusColor2, Weigth_plus.x);
			float4 pluslerp2 = lerp(plusColor3, plusColor4, Weigth_plus.x);
			plusColor = lerp(pluslerp1, pluslerp2, Weigth_plus.y);
		}
		GroupMemoryBarrierWithGroupSync();

		//gtxtRWFillters[level][TexCoord] = lerp(gtxtRWFillters[level][TexCoord], plusColor, 0.5f);

		if (frac(TexBuffer.x) < 0.000001f && frac(TexBuffer.y) < 0.000001f) {
			gtxtRWFillters[level][TexCoord] += lerp(gtxtRWFillters[level][TexCoord], plusColor, 0.5f);
			//gtxtRWFillters[level][TexCoord] += gtxtRWBlurs[level + 1][TexCoord_plus];
		}
		GroupMemoryBarrierWithGroupSync();

		if (frac(TexBuffer.x) < 0.000001f && frac(TexBuffer.y) < 0.000001f) {
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
					f4Color += gfGaussianBlurMask2D[i + 2][j + 2] * gtxtRWFillters[level][TexCoord + int2(i, j)];

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

	GroupMemoryBarrierWithGroupSync();

	float4 FinalBloomColor;


	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1,0)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 1)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 1)];
	//FinalBloomColor /= 4;

	int2 uv = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
	float2 uvWeight = frac(DTid / REDUTION_SIZE);

	float4 Color1 = gtxtRWBlurs[1][uv];
	float4 Color2 = gtxtRWBlurs[1][uv + int2(1, 0)];
	float4 Color3 = gtxtRWBlurs[1][uv + int2(0, 1)];
	float4 Color4 = gtxtRWBlurs[1][uv + int2(1, 1)];
	float4 lerp1 = lerp(Color1, Color2, uvWeight.x);
	float4 lerp2 = lerp(Color3, Color4, uvWeight.x);
	BloomColor = lerp(lerp1, lerp2, uvWeight.y);
	gtxtRWBlurs[0][DTid.xy] = BloomColor;

	gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
	//gtxtRWOutput[DTid.xy] = float4(lerp(Color.xyz, BloomColor.xyz, 0.5f), Color.w);

}