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
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
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

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};

float WeightSum(float4 fWeight)
{
	float result = 0.0f;

	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
		result += fWeight[i];
	
	return result;
}

VS_OUTPUT VS_Player(VS_INPUT input)
{
	VS_OUTPUT output;
	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	
	if (WeightSum(input.weights))
	{
		for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
		{
			mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		}

		output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
		output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
		output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
		output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
		output.uv = input.uv;
	}
	else
	{
		output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
		output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
		output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
		output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
		output.uv = input.uv;
	}

	return(output);
}