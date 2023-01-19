#include "Light.hlsl"
struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	input.normalW = normalize(input.normalW);
	float4 uvs[MAX_LIGHTS];
	float4 cIllumination = Lighting(input.positionW, input.normalW, false, uvs);

	//	return(cIllumination);
	return(float4(input.normalW * 0.5f + 0.5f, 1.0f));
}