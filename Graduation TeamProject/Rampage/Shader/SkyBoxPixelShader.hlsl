TextureCube gtxtSkyCubeTexture : register(t30);
SamplerState gssClamp : register(s4);
SamplerState gSamplerState : register(s0);

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gSamplerState, input.positionL);

	return(cColor);
}