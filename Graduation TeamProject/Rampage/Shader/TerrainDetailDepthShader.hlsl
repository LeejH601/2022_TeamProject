#include "Header.hlsli"

#define MAX_LIGHTS			4 

struct GS_DETAIL_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	float3 color : COLOR;
	float4 uvs[MAX_LIGHTS] : TEXCOORD2;
	//uint primID : SV_PrimitiveID;
	//int textureIndex : TEXTUREINDEX;
};

Texture2D gtxMappedTexture[8] : register(t0);
SamplerState gSamplerState : register(s0);

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Detail(GS_DETAIL_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float2 uv = input.uv;
	float4 cTexture = gtxMappedTexture[0].Sample(gSamplerState, uv);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	//cColor.a = 1.0f;
	//float4 cColor = float4(1,1,1,1); //  // cIllumination * cTexture;
	//cColor.rgb *= gfColor * 2.0f;
	//float4 cIllumination = Lighting(input.posW, input.normalW, cColor);

	if (cColor.a < 0.01f)
		discard;

	float Depth = input.posH.z;

	//output.f4Scene = cColor;
	output.f4CameraNormal = float4(Depth, Depth, Depth, Depth);
	//output.f4Scene = cColor;

	return(output);
}