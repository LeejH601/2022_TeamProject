#include "Header.hlsli"

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

struct GS_OUT
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
	uint index : INDEX;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT LensFlare_PS(GS_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float2 uv = float2(input.uv); // 수정 float3 uvw = float3(input.uv, input.primID ); 
	float4 cColor = gtxMappedTexture[input.index].Sample(gSamplerState, uv);
	//cColor.xyz *= 0.6f;
	//float4 cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	//cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경

	output.f4Color = cColor;
	//output.f4Color.w = 0.0f;

	return(output);
}