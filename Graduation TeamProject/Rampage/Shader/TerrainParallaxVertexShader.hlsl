struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

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
	float4 uvs[MAX_LIGHTS] : TEXCOORD4;
};

VS_TERRAIN_OUTPUT VSParallaxTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
	float3 normalW = mul(float4(input.normal, 0.0f), gmtxGameObject).xyz;
	output.normalW = normalW;
	output.position = mul(mul(positionW, gmtxView), gmtxProjection);
	output.positionW = positionW;

	float3x3 mtxTangentToWorld;
	mtxTangentToWorld[0] = mul(input.tangent, gmtxGameObject);
	mtxTangentToWorld[2] = mul(input.normal, gmtxGameObject);
	mtxTangentToWorld[1] = mul(cross(input.tangent, input.normal), gmtxGameObject);

	float3 toLight = float3(0.0f, 1.0f, 0.0f);
	[unroll(MAX_LIGHTS)] for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				toLight = normalize(mul(mtxTangentToWorld, gLights[i].m_vDirection));
				break;
			}
		}
	}

	output.toLight = toLight;
	output.toCamera = normalize(mul(mtxTangentToWorld, (gf3CameraPosition - positionW.xyz)));
	output.normal = normalize(mul(mtxTangentToWorld, normalW));

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gcbToLightSpaces[i].f4Position.w != 0.0f) output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
	}

	return(output);
}