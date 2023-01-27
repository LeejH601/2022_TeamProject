cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128

cbuffer cbBoneOffsets : register(b2)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b3)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

#include "Light.hlsl"

struct CB_TOOBJECTSPACE
{
	matrix		mtxToTexture;
	float4		f4Position;
};

cbuffer cbToLightSpace : register(b6)
{
	CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};


struct VS_INPUT
{
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_SHADOW_MAP_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;

	float4 uvs[MAX_LIGHTS] : TEXCOORD0;
};

float WeightSum(float4 fWeight)
{
	float result = 0.0f;

	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
		result += fWeight[i];

	return result;
}

VS_SHADOW_MAP_OUTPUT VS_Player_Shadow(VS_INPUT input)
{
	VS_SHADOW_MAP_OUTPUT output;
	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;

	if (WeightSum(input.weights))
	{
		for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
		{
			mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		}

		float4 positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
		output.positionW = positionW.xyz;
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
		output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			if (gcbToLightSpaces[i].f4Position.w != 0.0f) output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
		}
	}
	else
	{
		float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
		output.positionW = positionW.xyz;
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
		output.normalW = mul(input.normal, (float3x3)gmtxGameObject);

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			if (gcbToLightSpaces[i].f4Position.w != 0.0f) output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
		}
	}

	

	return(output);
}