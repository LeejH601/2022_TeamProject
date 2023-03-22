#define RESOULTION_X 1920
#define RESOULTION_Y 1080
#define REDUTION_SIZE 4

Texture2D gtxtSource : register(t50);
RWTexture2D<float4> gtxtRWOutput : register(u0);
RWTexture2D<float4> gtxtRWBlurs[5] : register(u10);

[numthreads(32, 32, 1)]
void BloomAdditive_CS( uint3 DTid : SV_DispatchThreadID )
{
	float BloomFactor = 0.5f;
	float4 Color = gtxtSource[DTid.xy];
	int2 uv = int2(DTid.x / REDUTION_SIZE, DTid.y / REDUTION_SIZE);
	float2 uvWeight = frac(DTid / REDUTION_SIZE);

	float4 Color1 = gtxtRWBlurs[1][uv];
	float4 Color2 = gtxtRWBlurs[1][uv + int2(1, 0)];
	float4 Color3 = gtxtRWBlurs[1][uv + int2(0, 1)];
	float4 Color4 = gtxtRWBlurs[1][uv + int2(1, 1)];
	float4 lerp1 = lerp(Color1, Color2, uvWeight.x);
	float4 lerp2 = lerp(Color3, Color4, uvWeight.x);
	float4 BloomColor = lerp(lerp1, lerp2, uvWeight.y);
	gtxtRWBlurs[0][DTid.xy] = BloomColor;


	gtxtRWOutput[DTid.xy] = float4((Color.xyz + (BloomColor.xyz * BloomFactor)), Color.w);
}