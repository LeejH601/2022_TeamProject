struct ResourceIndexInfo {
	int Texture_base_index;
	int Texture_normal_map_index;
	int Material_index;
};

cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
	ResourceIndexInfo textureInfo : packoffset(c4);
}

//----------------------------------------------------------------------------
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
}

cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
	ResourceIndexInfo ResourceInfo : packoffset(c4);
}

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VS_Tmp(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	return(output);
}