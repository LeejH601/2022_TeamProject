Texture2D gtxtTerrainTexture : register(t0);
Texture2D gtxtDetailTexture : register(t1);
Texture2D gtxtAlphaTexture : register(t2);


Texture2D gtxTerrainTextures[9] : register(t10);


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


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrainSplat(VS_TERRAIN_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float4 cBaseTexColor[4];
	float4 cNormalColor[4];
	int index = 8;
	float4 cAlphaColor = gtxTerrainTextures[8].Sample(gSamplerState, input.uv1);

	float4 cColor;

	for (int i = 0; i < 4; ++i) {
		cBaseTexColor[i] = gtxTerrainTextures[i].Sample(gSamplerState, input.uv0);
		cNormalColor[i] = gtxTerrainTextures[4+i].Sample(gSamplerState, input.uv0);

		if (i == 0)
			cColor += cBaseTexColor[i] * cAlphaColor.r;
		else if (i == 1)
			cColor += cBaseTexColor[i] * cAlphaColor.g;
		else if (i == 2)
			cColor += cBaseTexColor[i] * cAlphaColor.b;
		else if (i == 3)
			cColor += cBaseTexColor[i] * cAlphaColor.a;
	}

	output.f4Scene = cColor;
	output.f4ImGui = cColor;

	return output;
}