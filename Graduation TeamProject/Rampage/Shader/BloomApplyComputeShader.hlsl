
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
	for (int level = 3; level > 0; --level) {

		float2 Weigth = float2(DTid.x % pow(REDUTION_SIZE, level), DTid.y % pow(REDUTION_SIZE, level));
		float2 DeltaUV = float2(1.0f / (RESOULTION_X / pow(REDUTION_SIZE, level)), 1.0f / (RESOULTION_Y / pow(REDUTION_SIZE, level)));
		float2 Delta = float2(1.0f / RESOULTION_X, 1.0f / RESOULTION_Y);
		int2 TexCoord = int2(DTid.x / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		int2 TexCoord1 = int2((DTid.x + Delta.x) / pow(REDUTION_SIZE, level), DTid.y / pow(REDUTION_SIZE, level));
		int2 TexCoord2 = int2(DTid.x / pow(REDUTION_SIZE, level), (DTid.y + Delta.y) / pow(REDUTION_SIZE, level));
		int2 TexCoord3 = int2((DTid.x + Delta.x) / pow(REDUTION_SIZE, level), (DTid.y + Delta.y) / pow(REDUTION_SIZE, level));

		float4 f4Color = float4(0, 0, 0, 0);

		{
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					//f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtRWFillters[level][ int2(float2(TexCoord.xy) + DeltaUV)];
					f4Color += gfGaussianBlurMask2D_2[i + 2][j + 2] * gtxtRWFillters[level][TexCoord.xy + int2(i, j)];

				}
			}
			gtxtRWBlurs[level][TexCoord] = f4Color;
		}

		GroupMemoryBarrierWithGroupSync();


		//f4Color += gtxtRWBlurs[level].SampleLevel(gSamplerState, uv, 0);
		/*f4Color += gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(0, 0))];
		f4Color += gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(DeltaUV.x, 0))];
		f4Color += gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(0, DeltaUV.y))];
		f4Color += gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(DeltaUV.x, DeltaUV.y))];
		f4Color /= 4;*/

	/*	float4 f4Color_1 = gtxtRWBlurs[level][TexCoord.xy];
		float4 f4Color_2 = gtxtRWBlurs[level][TexCoord.xy + int2(1, 0)];
		float4 f4Color_3 = gtxtRWBlurs[level][TexCoord.xy + int2(0, 1)];
		float4 f4Color_4 = gtxtRWBlurs[level][TexCoord.xy + int2(1, 1)];
		float4 f4lerp1 = lerp(f4Color_1, f4Color_2, Weigth.x);
		float4 f4lerp2 = lerp(f4Color_3, f4Color_4, Weigth.x);
		
		f4Color = lerp(f4lerp1, f4lerp2, Weigth.y);*/

		/*float4 f4Color_1 = gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(0, 0))];
		float4 f4Color_2 = gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(DeltaUV.x, 0))];
		float4 f4Color_3 = gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(0, DeltaUV.y))];
		float4 f4Color_4 = gtxtRWBlurs[level][int2(float2(TexCoord.xy) + float2(DeltaUV.x, DeltaUV.y))];
		float4 f4lerp1 = lerp(f4Color_1, f4Color_2, uvWeight.x);
		float4 f4lerp2 = lerp(f4Color_3, f4Color_4, uvWeight.x);

		f4Color = lerp(f4lerp1, f4lerp2, uvWeight.y);*/

		/*f4Color += gtxtRWBlurs[level][TexCoord.xy];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1,0)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(0,1)];
		f4Color += gtxtRWBlurs[level][TexCoord.xy + int2(1,1)];
		f4Color /= 4;*/

		/*f4Color += gtxtRWBlurs[level][TexCoord.xy];
		f4Color += gtxtRWBlurs[level][TexCoord1];
		f4Color += gtxtRWBlurs[level][TexCoord2];
		f4Color += gtxtRWBlurs[level][TexCoord3];
		f4Color /= 4;*/

		//f4Color += gtxtRWBlurs[level][TexCoord.xy];

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


/*if (level > 2) {
	Color += 0.06f * f4Color;
}
else if (level > 3) {
	Color += 0.03f * f4Color;
}
else*/
		//Color.xyz += BloomFactor * f4Color.xyz;
		//BloomColor.xyz = lerp(BloomColor.xyz, f4Color.xyz, 0.5f);
		//BloomFactor -= 0.01f;
	}
	gtxtRWBlurs[0][DTid.xy] = BloomColor;

	GroupMemoryBarrierWithGroupSync();

	float4 FinalBloomColor;

	// 3°³ Æò±Õ
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(-1, -1)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, -1)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, -1)];

	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(-1, 0)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 0)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 0)];

	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(-1, 1)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 1)];
	FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 1)];
	FinalBloomColor /= 9;

	
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1,0)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(0, 1)];
	//FinalBloomColor = gtxtRWBlurs[0][DTid.xy + int2(1, 1)];
	//FinalBloomColor /= 4;




	gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
	//gtxtRWOutput[DTid.xy] = float4(lerp(Color.xyz, BloomColor.xyz, 0.5f), Color.w);

}