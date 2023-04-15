
Texture2D gtxtSource : register(t50);

RWTexture2D<float4> gtxtRWOutput : register(u0);

float3 ToneMapACES(float3 hdr) {
	const float A = 2.51f, B = 0.03f, C = 2.43, D = 0.59, E = 0.14f;
	return saturate((hdr * (A * hdr + B)) / (hdr * (C * hdr + D) + E));
}

float3 aces_approx(float3 v)
{
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v * (a * v + b)) / (v * (c * v + d) + e), 0.0f, 1.0f);
}

float3 uncharted2_tonemap_partial(float3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 uncharted2_filmic(float3 v)
{
    float exposure_bias = 2.0f;
    float3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    float3 W = float3(11.2f, 11.2f, 11.2f);
    float3 white_scale = float3(1.0f, 1.0f, 1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

#define UNCHARTED2_TONE_MAPPING
#define GAMMA

[numthreads(32, 32, 1)]
void HDR_CS(int3 nDispatchID : SV_DispatchThreadID)
{
    float4 SourceColor = gtxtSource[nDispatchID.xy];
    SourceColor = max(0.0f, SourceColor);
    SourceColor.xyz *= 2.0f;

    float4 LDRColor;
#ifdef UNCHARTED2_TONE_MAPPING
    LDRColor = float4(uncharted2_filmic(SourceColor.xyz), SourceColor.w);
#else
	//gtxtRWOutput[nDispatchID.xy] = lerp(gtxtInputA[nDispatchID.xy], gtxtInputB[nDispatchID.xy], 0.35f);
	//gtxtRWOutput[nDispatchID.xy] = float4(ToneMapACESSourceColor.xyz), SourceColor.w);
    LDRColor = float4(aces_approx(SourceColor.xyz), SourceColor.w);
	//gtxtRWOutput[nDispatchID.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
#endif

#ifdef GAMMA
    float gamma = 0.7; // gamma value to use for correction
    float4 correctedColor = float4(pow(LDRColor.xyz, 1.0 / gamma), 1.0f);
    //correctedColor = min(float4(1.0f,1.0f,1.0f,1.0f), correctedColor);
    gtxtRWOutput[nDispatchID.xy] = correctedColor;
#else
    //LDRColor = min(float4(1.0f, 1.0f, 1.0f, 1.0f), LDRColor);
    gtxtRWOutput[nDispatchID.xy] = saturate(LDRColor);
#endif
}