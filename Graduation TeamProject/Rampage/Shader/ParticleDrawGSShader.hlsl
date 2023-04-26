struct VS_PARTICLE_DRAW_OUTPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float4 color : COLOR;
	float2 size : SCALE;
	float alpha : ALPHA;
	int  bTerrain : TERRAIN;
};

struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	float alpha : ALPHA;
};

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4);
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	matrix gmtxInverseView : packoffset(c12);
	float3 gf3CameraPosition : packoffset(c16);
	//float3 gf3CameraDirection : packoffset(c17);
};

static float3 gf3Positions[4] = { float3(-1.0f, +1.0f, 0.0f), float3(+1.0f, +1.0f, 0.0f), float3(-1.0f, -1.0f, 0.0f), float3(+1.0f, -1.0f, 0.0f) };
static float3 gf3PositionsTerrain[4] = { float3(-1.0f, 0.0f, +1.0f), float3(+1.0f, 0.0f, +1.0f), float3(-1.0f, 0.0f, -1.0f), float3(+1.0f, 0.0f, -1.0f) };
//static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };
static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };
[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_DRAW_OUTPUT input[1], inout TriangleStream<GS_PARTICLE_DRAW_OUTPUT> outputStream)
{
	GS_PARTICLE_DRAW_OUTPUT output = (GS_PARTICLE_DRAW_OUTPUT)0;

	//float3 vLookCamera = gf3CameraPosition - input[0].position;
	//vLookCamera = normalize(vLookCamera);
	//float3 direction = normalize(input[0].velocity);
	//float3 right = cross(float3(0, 1, 0), direction);
	//float3 up = cross(direction, right);

	//matrix rotationMatrix;
	//rotationMatrix[0] = float4(right, 0);
	//rotationMatrix[1] = float4(up, 0);
	//rotationMatrix[2] = float4(direction, 0);
	//rotationMatrix[3] = float4(0, 0, 0, 1);

	//rotationMatrix = transpose(rotationMatrix);
	output.alpha = input[0].alpha;
	output.color = input[0].color;
	for (int i = 0; i < 4; i++)
	{
		if (input[0].bTerrain)
		{
			gf3Positions[i] = gf3PositionsTerrain[i];
		}
		gf3Positions[i].x = gf3Positions[i].x * input[0].size.x * 0.5f;
		gf3Positions[i].y = gf3Positions[i].y * input[0].size.y * 0.5f;


		//gf3Positions[i] = mul(float4(gf3Positions[i], 1.f), rotationMatrix).xyz;
		//gf3Positions[i] = mul(rotationMatrix, float4(gf3Positions[i], 1.f)).xyz;
		float3 positionW = mul((gf3Positions[i]), (float3x3)(gmtxInverseView)) + input[0].position;
		

		output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
		output.uv = mul(float3(gf2QuadUVs[i], 1.0f), (float3x3)(gmtxGameObject)).xy;
		outputStream.Append(output);
	}
	outputStream.RestartStrip();
}


//inline XMMATRIX XM_CALLCONV XMMatrixLookToLH
//(
//	FXMVECTOR EyePosition,
//	FXMVECTOR EyeDirection,
//	FXMVECTOR UpDirection
//)
//{
//	assert(!XMVector3Equal(EyeDirection, XMVectorZero()));
//	assert(!XMVector3IsInfinite(EyeDirection));
//	assert(!XMVector3Equal(UpDirection, XMVectorZero()));
//	assert(!XMVector3IsInfinite(UpDirection));
//
//	XMVECTOR R2 = XMVector3Normalize(EyeDirection);
//
//	XMVECTOR R0 = XMVector3Cross(UpDirection, R2);
//	R0 = XMVector3Normalize(R0);
//
//	XMVECTOR R1 = XMVector3Cross(R2, R0);
//
//	XMVECTOR NegEyePosition = XMVectorNegate(EyePosition);
//
//	XMVECTOR D0 = XMVector3Dot(R0, NegEyePosition);
//	XMVECTOR D1 = XMVector3Dot(R1, NegEyePosition);
//	XMVECTOR D2 = XMVector3Dot(R2, NegEyePosition);
//
//	XMMATRIX M;
//	M.r[0] = XMVectorSelect(D0, R0, g_XMSelect1110.v);
//	M.r[1] = XMVectorSelect(D1, R1, g_XMSelect1110.v);
//	M.r[2] = XMVectorSelect(D2, R2, g_XMSelect1110.v);
//	M.r[3] = g_XMIdentityR3.v;
//
//	M = XMMatrixTranspose(M);
//
//	return M;
//}
