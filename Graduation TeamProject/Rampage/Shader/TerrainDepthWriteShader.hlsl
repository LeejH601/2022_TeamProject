struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 positionW : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 normalW : NORMAL1;
	float3 toCamera : TEXCOORD2;
	float3 toLight : TEXCOORD3;
};

struct PS_DEPTH_OUTPUT
{
	float fzPosition : SV_Target;
	float fDepth : SV_Depth;
};

PS_DEPTH_OUTPUT PS_TerrainDepthWriteShader(VS_TERRAIN_OUTPUT input)
{
	PS_DEPTH_OUTPUT output;

	output.fzPosition = input.position.z;
	output.fDepth = input.position.z;

	return(output);
}