cbuffer LevelInfo : register(b0)
{
	float4 gnLevels : packoffset(c0);
}

#define RESOULTION_X 1920
#define RESOULTION_Y 1080
//#define REDUTION_SIZE 4

Texture2D gtxtSource : register(t50);
RWTexture2D<float4> gtxtRWOutput : register(u0);
RWTexture2D<float4> gtxtRWFillters[5] : register(u5);
RWTexture2D<float4> gtxtRWBlurs[5] : register(u10);

static float gfGaussianBlurMask2D[5][5] = {
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
	{ 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
	{ 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }
};

static float gfGaussianBlurMask2D77[7][7] = {
	{0.05854983, 0.07729707, 0.09131569, 0.09653235, 0.09131569, 0.07729707, 0.05854983},
	{0.07729707, 0.10204703, 0.12055432, 0.12744132, 0.12055432, 0.10204703, 0.07729707},
	{0.09131569, 0.12055432, 0.1424181 , 0.15055413, 0.1424181 , 0.12055432, 0.09131569},
	{0.09653235, 0.12744132, 0.15055413, 0.15915494, 0.15055413, 0.12744132, 0.09653235},
	{0.09131569, 0.12055432, 0.1424181 , 0.15055413, 0.1424181 , 0.12055432, 0.09131569},
	{0.07729707, 0.10204703, 0.12055432, 0.12744132, 0.12055432, 0.10204703, 0.07729707},
	{0.05854983, 0.07729707, 0.09131569, 0.09653235, 0.09131569, 0.07729707, 0.05854983},
};

static float gfGaussianBlurMask2D99[9][9] = {
	{0.05854983, 0.07286644, 0.0851895 , 0.09356236, 0.09653235, 0.09356236, 0.0851895 , 0.07286644, 0.05854983},
	{0.07286644, 0.09068375, 0.10602005, 0.11644024, 0.12013645, 0.11644024, 0.10602005, 0.09068375, 0.07286644},
	{0.0851895 , 0.10602005, 0.12394999, 0.13613244, 0.14045374, 0.13613244, 0.12394999, 0.10602005, 0.0851895},
	{0.09356236, 0.11644024, 0.13613244, 0.14951223, 0.15425826, 0.14951223, 0.13613244, 0.11644024, 0.09356236},
	{0.09653235, 0.12013645, 0.14045374, 0.15425826, 0.15915494, 0.15425826, 0.14045374, 0.12013645, 0.09653235},
	{0.09356236, 0.11644024, 0.13613244, 0.14951223, 0.15425826, 0.14951223, 0.13613244, 0.11644024, 0.09356236},
	{0.0851895 , 0.10602005, 0.12394999, 0.13613244, 0.14045374, 0.13613244, 0.12394999, 0.10602005, 0.0851895},
	{0.07286644, 0.09068375, 0.10602005, 0.11644024, 0.12013645, 0.11644024, 0.10602005, 0.09068375, 0.07286644},
	{0.05854983, 0.07286644, 0.0851895 , 0.09356236, 0.09653235, 0.09356236, 0.0851895 , 0.07286644, 0.05854983},
};


//#define BICUBIC

[numthreads(32, 32, 1)]
void BloomAdditive_CS(uint3 DTid : SV_DispatchThreadID)
{
	int REDUTION_SIZE = (int)gnLevels.y;
	float4 Color = gtxtSource[DTid.xy];
	int2 uv = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);

	int2 uv1 = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
	int2 uv2 = int2((DTid.x + 1) / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
	int2 uv3 = int2(DTid.x / REDUTION_SIZE, (DTid.y + 1) / REDUTION_SIZE);
	int2 uv4 = int2((DTid.x + 1) / REDUTION_SIZE,( DTid.y + 1) / REDUTION_SIZE);
	float2 uvWeight = frac(DTid / REDUTION_SIZE);
	float4 BloomColor;

#ifdef BICUBIC
	{
		float4 plusColor1 = gtxtRWBlurs[1][uv + int2(-1, -1)];
		float4 plusColor2 = gtxtRWBlurs[1][uv + int2(0, -1)];
		float4 plusColor5 = gtxtRWBlurs[1][uv + int2(-1, 0)];
		float4 plusColor6 = gtxtRWBlurs[1][uv + int2(0, 0)];

		float4 plusColor3 = gtxtRWBlurs[1][uv + int2(1, -1)];
		float4 plusColor4 = gtxtRWBlurs[1][uv + int2(2, 1)];
		float4 plusColor7 = gtxtRWBlurs[1][uv + int2(1, 0)];
		float4 plusColor8 = gtxtRWBlurs[1][uv + int2(2, 1)];

		float4 plusColor9 = gtxtRWBlurs[1][uv + int2(-1, 1)];
		float4 plusColor10 = gtxtRWBlurs[1][uv + int2(0, 1)];
		float4 plusColor13 = gtxtRWBlurs[1][uv + int2(-1, 2)];
		float4 plusColor14 = gtxtRWBlurs[1][uv + int2(0, 2)];

		float4 plusColor11 = gtxtRWBlurs[1][uv + int2(1, 1)];
		float4 plusColor12 = gtxtRWBlurs[1][uv + int2(2, 1)];
		float4 plusColor15 = gtxtRWBlurs[1][uv + int2(1, 2)];
		float4 plusColor16 = gtxtRWBlurs[1][uv + int2(2, 2)];

		//
		float4 pluslerp12 = lerp(plusColor1, plusColor2, uvWeight.x);
		float4 pluslerp56 = lerp(plusColor5, plusColor6, uvWeight.x);

		float4 pluslerp23 = lerp(plusColor2, plusColor3, uvWeight.x);
		float4 pluslerp67 = lerp(plusColor6, plusColor7, uvWeight.x);

		float4 pluslerp34 = lerp(plusColor3, plusColor4, uvWeight.x);
		float4 pluslerp78 = lerp(plusColor7, plusColor8, uvWeight.x);

		float4 pluslerp910 = lerp(plusColor9, plusColor10, uvWeight.x);
		float4 pluslerp1314 = lerp(plusColor13, plusColor14, uvWeight.x);

		float4 pluslerp1011 = lerp(plusColor10, plusColor11, uvWeight.x);
		float4 pluslerp1415 = lerp(plusColor14, plusColor15, uvWeight.x);

		float4 pluslerp1112 = lerp(plusColor11, plusColor12, uvWeight.x);
		float4 pluslerp1516 = lerp(plusColor15, plusColor16, uvWeight.x);

		//
		float4 LT = lerp(pluslerp12, pluslerp56, uvWeight.y);
		float4 TC = lerp(pluslerp23, pluslerp67, uvWeight.y);
		float4 RT = lerp(pluslerp34, pluslerp78, uvWeight.y);

		float4 L = lerp(pluslerp56, pluslerp910, uvWeight.y);
		float4 C = lerp(pluslerp67, pluslerp1011, uvWeight.y);
		float4 R = lerp(pluslerp78, pluslerp1112, uvWeight.y);

		float4 LB = lerp(pluslerp910, pluslerp1314, uvWeight.y);
		float4 BC = lerp(pluslerp1011, pluslerp1415, uvWeight.y);
		float4 RB = lerp(pluslerp1112, pluslerp1516, uvWeight.y);

		//
		float LTTC = lerp(LT, TC, uvWeight.x);
		float TCRT = lerp(TC, RT, uvWeight.x);

		float LC = lerp(L, C, uvWeight.x);
		float CR = lerp(C, R, uvWeight.x);

		float LBBC = lerp(LB, BC, uvWeight.x);
		float RCRB = lerp(BC, RB, uvWeight.x);

		//
		float4 S1 = lerp(LTTC, LC, uvWeight.y);
		float4 S2 = lerp(TCRT, CR, uvWeight.y);
		float4 S3 = lerp(LC, LBBC, uvWeight.y);
		float4 S4 = lerp(CR, RCRB, uvWeight.y);

		//
		float4 S1S2 = lerp(S1, S2, uvWeight.x);
		float4 S3S4 = lerp(S3, S4, uvWeight.x);

		//
		BloomColor = lerp(S1S2, S3S4, uvWeight.y);
	}
#else
	{
		float4 Color1 = gtxtRWBlurs[1][uv];
		float4 Color2 = gtxtRWBlurs[1][uv + int2(1, 0)];
		float4 Color3 = gtxtRWBlurs[1][uv + int2(0, 1)];
		float4 Color4 = gtxtRWBlurs[1][uv + int2(1, 1)];
		float4 lerp1 = lerp(Color1, Color2, uvWeight.x);
		float4 lerp2 = lerp(Color3, Color4, uvWeight.x);
		BloomColor = lerp(lerp1, lerp2, uvWeight.y);

		/*float4 Color = gtxtRWBlurs[1][uv1];
		Color += gtxtRWBlurs[1][uv2];
		Color += gtxtRWBlurs[1][uv3];
		Color += gtxtRWBlurs[1][uv4];
		BloomColor = Color / 4;*/
	}
#endif


	float f4Color = float4(0, 0, 0, 0);

	//BloomColor = gtxtRWBlurs[1][uv];
	//BloomColor = f4Color;

	float BloomFactor = 0.3f;

	BloomColor *= BloomFactor;

	gtxtRWBlurs[0][DTid.xy] = float4(BloomColor.xyz, 0.0f);



	//gtxtRWOutput[DTid.xy] =  float4(max(gtxtRWOutput[DTid.xy].xyz, (BloomColor.xyz)), Color.w);
	gtxtRWOutput[DTid.xy] = float4((Color.xyz + BloomColor.xyz), Color.w);
}