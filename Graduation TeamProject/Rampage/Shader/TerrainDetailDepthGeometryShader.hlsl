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
	float2 size : SIZE;
	float3 color : COLOR;
};


struct GS_DETAIL_OUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	float3 color : COLOR;
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
	float3 vLook = gf3CameraPosition.xyz - centerW;

	float fHalfW = input[0].size.x * 0.5f;
	float fHalfH = input[0].size.y * 0.5f;
	float4 pVertices[4];

	vLook = normalize(vLook);
	float3 vRight = cross(float3(0.f, 1.0f, 0.0f), vLook);

	float2 pUVs[4] = { float2(0.f, 0.f), float2(1.f, 0.f), float2(0.f, 1.f), float2(1.f, 1.f) };
	GS_DETAIL_OUT output;
	output.color = input[0].color;

	/*gf3Positions[0] = float4(center.x + input[0].size.x, center.y, center.z - input[0].size.y, 1.0f);
	gf3Positions[1] = float4(center.x + input[0].size.x, center.y, center.z + input[0].size.y, 1.0f);
	gf3Positions[2] = float4(center.x - input[0].size.x, center.y, center.z - input[0].size.y, 1.0f);
	gf3Positions[3] = float4(center.x - input[0].size.x, center.y, center.z + input[0].size.y, 1.0f);*/

	float3 textDir = float3(1.0f, 0.0f, 0.0f);
	float value = 0.1f;

	textDir = mul(float4(textDir, 0.0f), gmtxView);

	for (int i = 0; i < 4; i++)
	{
		gf3Positions[i].x = gf3Positions[i].x * fHalfW;
		gf3Positions[i].y = gf3Positions[i].y * fHalfH;

		output.posH = mul(float4(centerW, 1.0f), gmtxView);
		output.posH.xyz += textDir * sin(gfCurrentTime) * value * (1-pUVs[i].y);
		output.posH.xyz += gf3Positions[i];
		output.posH = mul(output.posH, gmtxProjection);

		output.posW = output.posH.xyz;
		output.normalW = vLook;
		output.uv = pUVs[i];
		//output.uv = mul(float3(gf2QuadUVs[i], 1.0f), (float3x3)(xmf4x4Coord)).xy;
		outputStream.Append(output);
	}

	
}