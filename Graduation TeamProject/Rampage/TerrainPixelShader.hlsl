Texture2D gtxtTerrainTexture : register(t0);
Texture2D gtxtDetailTexture : register(t1);
Texture2D gtxtAlphaTexture : register(t2);

SamplerState gSamplerState : register(s0);

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer
	float4 f4ImGui : SV_TARGET1;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	float4 cBaseTexColor = gtxtTerrainTexture.Sample(gSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtDetailTexture.Sample(gSamplerState, input.uv1);
	//	float fAlpha = gtxtTerrainTexture.Sample(gssWrap, input.uv0);

	float4 cColor = cBaseTexColor * 0.5f + cDetailTexColor * 0.5f;
	//	float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));

	output.f4Scene = cColor;
	output.f4ImGui = cColor;

	return(output);
}
