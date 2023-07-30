#define MAX_LIGHTS			4 

cbuffer cbFrameworkInfo : register(b7)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	float		gfLifeTime : packoffset(c0.z);
	bool		bEmit : packoffset(c0.w);

	uint2		iTextureCoord : packoffset(c1.x);
	uint		iTextureIndex : packoffset(c1.z);
	uint		gnParticleType : packoffset(c1.w);

	float3		gf3Gravity : packoffset(c2.x);
	float		gfSpeed : packoffset(c2.w);

	float3		gfColor : packoffset(c3.x);
	uint		gnFlareParticlesToEmit : packoffset(c3.w);

	float2		gfSize : packoffset(c4.x);

};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix m_xmf4x4OrthoProjection : packoffset(c8);
	matrix gmtxInverseProjection : packoffset(c12);
	matrix gmtxInverseView : packoffset(c16);
	float3 gf3CameraPosition : packoffset(c20);
	//float3 gf3CameraDirection : packoffset(c17);
};

struct VS_DETAIL_OUTPUT
{
	float4 position : SV_POSITION;
	float3 size : SIZE;
	float3 color : COLOR;
	float3 normal : NORMAL;
	float4 uvs[MAX_LIGHTS] : TEXCOORD1;
};


struct GS_DETAIL_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	float3 color : COLOR;
	float4 uvs[MAX_LIGHTS] : TEXCOORD2;
	//uint primID : SV_PrimitiveID;
	//int textureIndex : TEXTUREINDEX;
};

#define MAX_DETAILS_INSTANCES_ONE_DRAW_CALL 4000
cbuffer cbDetailInfo : register(b7)
{
	float4 gnSizeAndOffset;
	float3 gnDetailPoisitions[MAX_DETAILS_INSTANCES_ONE_DRAW_CALL];
};

static float3 gf3Positions[4] = { float3(-1.0f, +1.0f, 0.0f), float3(+1.0f, +1.0f, 0.0f), float3(-1.0f, -1.0f, 0.0f), float3(+1.0f, -1.0f, 0.0f) };

[maxvertexcount(4)]
void GS_Detail(
	point VS_DETAIL_OUTPUT input[1] : SV_POSITION,
	inout TriangleStream< GS_DETAIL_OUT > outputStream
)
{
	float3 centerW = input[0].position.xyz;
	centerW.y += input[0].size.y / 2.0f;
	float3 vUp = float3(0.f, 1.0f, 0.0f);
	//float3 vLook = gf3CameraPosition.xyz - centerW;
	float3 vLook = input[0].normal;

	float fHalfW = input[0].size.x * 0.5f;
	float fHalfH = input[0].size.y * 0.5f;
	float4 pVertices[4];

	vLook = normalize(vLook);
	float3 vRight = cross(float3(0.f, 1.0f, 0.0f), vLook);
	float3 vWorldOffsets[4] = { -vRight + vUp, vRight + vUp, -vRight - vUp, vRight - vUp };

	float2 pUVs[4] = { float2(0.f, 0.f), float2(1.f, 0.f), float2(0.f, 1.f), float2(1.f, 1.f) };
	GS_DETAIL_OUT output;
	output.color = input[0].color;
	output.uvs = input[0].uvs;

	float3 WindDir = float3(1.0f, 0.0f, 0.0f) * input[0].size.z;
	float value = 0.1f;

	WindDir = mul(float4(WindDir, 0.0f), gmtxView);

	for (int i = 0; i < 4; i++)
	{
		vWorldOffsets[i].x = vWorldOffsets[i].x * fHalfW;
		vWorldOffsets[i].y = vWorldOffsets[i].y * fHalfH;

		output.posH = float4(centerW, 1.0f);
		output.posH.xyz += WindDir * sin(gfCurrentTime) * value * (1-pUVs[i].y);
		output.posH.xyz += vWorldOffsets[i];
		output.posH = mul(float4(output.posH.xyz, 1.0f), gmtxView);
		output.posW = centerW.xyz;


		output.posH = mul(output.posH, gmtxProjection);

		output.normalW = input[0].normal;
		output.uv = pUVs[i];
		outputStream.Append(output);
	}

	
}