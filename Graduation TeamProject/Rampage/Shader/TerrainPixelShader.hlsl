
Texture2D<float4> gtxtTerrainBaseTexture : register(t7);
Texture2D<float4> gtxtTerrainDetailTextures[3] : register(t8); //t8, t9, t10
//Texture2D<float> gtxtTerrainAlphaTexture : register(t5);
Texture2D<float4> gtxtTerrainAlphaTexture : register(t11);
SamplerState gSamplerState : register(s0);



struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float fAlpha = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0);

	float4 cDetailTexColors[3];
	cDetailTexColors[0] = gtxtTerrainDetailTextures[0].Sample(gSamplerState, input.uv1 * 2.0f);
	cDetailTexColors[1] = gtxtTerrainDetailTextures[1].Sample(gSamplerState, input.uv1 * 0.125f);
	cDetailTexColors[2] = gtxtTerrainDetailTextures[2].Sample(gSamplerState, input.uv1);

	float4 cColor = cBaseTexColor * cDetailTexColors[0];
	float fAlpha = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0).w;
	cColor += lerp(cDetailTexColors[1] * 0.25f, cDetailTexColors[2], 1.0f - fAlpha);
	/*
		cColor = lerp(cDetailTexColors[0], cDetailTexColors[2], 1.0f - fAlpha) ;
		cColor = lerp(cBaseTexColor, cColor, 0.3f) + cDetailTexColors[1] * (1.0f - fAlpha);
	*/
	/*
		if (fAlpha < 0.35f) cColor = cDetailTexColors[2];
		else if (fAlpha > 0.8975f) cColor = cDetailTexColors[0];
		else cColor = cDetailTexColors[1];
	*/
	return(cColor);

}

