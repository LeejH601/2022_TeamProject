
Texture2D gtxtSource : register(t50);

RWTexture2D<float4> gtxtRWOutput : register(u0);

[numthreads(32, 32, 1)]
void HDR_CS(int3 nDispatchID : SV_DispatchThreadID)
{
	//gtxtRWOutput[nDispatchID.xy] = lerp(gtxtInputA[nDispatchID.xy], gtxtInputB[nDispatchID.xy], 0.35f);
	gtxtRWOutput[nDispatchID.xy] = gtxtSource[nDispatchID.xy];
	//gtxtRWOutput[nDispatchID.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}