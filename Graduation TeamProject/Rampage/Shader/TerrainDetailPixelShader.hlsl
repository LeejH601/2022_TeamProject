#include "Header.hlsli"

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


#include "Light.hlsl"

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

Texture2D gtxMappedTexture[8] : register(t0);
Texture2D gtxMultiRenderTargetTextures[7] : register(t35);
SamplerState gSamplerState : register(s0);

static float width = 1920;
static float height = 1080;
static float ratio = 1920.f / 1080.f;
static float PI = 3.141592f;
static float Far = 500.0f;
static float near = 1.0f;
static matrix toViewPortMatrix = {
	width / 2, 0,	0,		-1 + width/2,
	0,	(-height)/2,	0,	-1 + height/2,
	0,	0,	1.0 - 0.0f,	 0,
	0,0, 0, 1
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PS_Detail(GS_DETAIL_OUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	float2 uv = input.uv;
	float4 cTexture = gtxMappedTexture[0].Sample(gSamplerState, uv);
	float4 cColor = cTexture; //  // cIllumination * cTexture;
	cColor.rgb *= input.color;

	if (cColor.a < 0.01f)
		discard;

	float alpha = cColor.a;

	float3 lightNormal = gLights[0].m_vPosition - input.posW;
	lightNormal = normalize(lightNormal);
	//cColor.a = 1.0f;
	//float4 cColor = float4(1,1,1,1); //  // cIllumination * cTexture;
	//cColor.rgb *= gfColor * 2.0f;
	float4 cIllumination = Lighting(input.posW, lightNormal, cColor, true, input.uvs);
	cIllumination.xyz /= lerp(1.0f, 4.0f, input.uv.y);
	cIllumination.w = alpha;
	//cIllumination = saturate(cIllumination);


	float3 Projection = input.posH.xyz;
	float Depth = Projection.z;
	//float2 screanUV = mul(float4(Projection, 1.0f), toViewPortMatrix).xy;
	float2 screanUV = float2(Projection.x / width, Projection.y /height);
	//screanUV.y = 1.0f - screanUV.y; 
	float4 DepthTexture = gtxMultiRenderTargetTextures[5].Sample(gSamplerState, screanUV.xy);
	if (DepthTexture.r < Depth)
		discard;
	/*if (0.02 > Depth)
		discard;*/
	//cColor.a = Depth;

	//output.f4Scene = cColor;
	output.f4Color = cIllumination;
	//output.f4Scene = DepthTexture;
	//output.f4Scene = float4(screanUV.x , screanUV.y , Projection.z,1.0f);
	output.f4Scene = float4(DepthTexture.r, 0 , Depth,1.0f);
	//output.f4Color = float4(DepthTexture.r, 0 , 0,1.0f);
	//output.f4Color = float4(screanUV.x , screanUV.y , Projection.z,1.0f);

	return(output);
}