#include "Header.hlsli"
#include "InputLayouts.hlsli"
#include "CurlNoise.hlsl"

Buffer<float4> gRandomSphereBuffer : register(t50);
Texture2D gtxtTexture[29] : register(t51);

SamplerState gSamplerState : register(s0);

cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxGameObject : packoffset(c0);
	matrix gmtxTexture : packoffset(c4); // ParticleIndex로 사용 (_11)
	uint gnTexturesMask : packoffset(c8); // 빌보드 알파값 사용(Billboard_PS)
};

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

#define MAX_TRAILCONTROLLPOINT 200
#define MAX_COLORCURVES 8
cbuffer cbTrailControllPoints : register(b5)
{
	float4 gControllpoints1[MAX_TRAILCONTROLLPOINT];
	float4 gControllpoints2[MAX_TRAILCONTROLLPOINT];
	float4 gfR_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfG_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfB_CurvePoints[MAX_COLORCURVES / 4];
	float4 gfColorCurveTimes[MAX_COLORCURVES / 4];
	uint gnCurves;
	float gfNoiseConstants;
	uint gnPoints;
	float gnOffsetTime;
	float gnEmissiveFactor;
	uint gnBaseTextureIndex;
	uint gnNoiseTextureIndex;
}


PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input)  // SphereTexture t50을 제외
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	//float4 cColor = gtxtTexture[0].Sample(gSamplerState, input.uv); // 2번이
	float4 cColor = gtxtTexture[(uint)(input.TextureIndex)].Sample(gSamplerState, input.uv); // 2번이
	if (input.ParticleType == 3) {
		int index = 0;
		float ColorUV = frac(((gfCurrentTime - input.EmitTime) / input.lifetime) + (input.positionW.x * 0.5f + 0.5f));
			for (int i = 0; i < gnCurves; ++i) {
				if (gfColorCurveTimes[i / 4][i % 4] >= ColorUV) {
					index = max(0, i - 1);
					break;
				}
			}

		float t = ColorUV - gfColorCurveTimes[index / 4][index % 4];
		t *= 1.0f / ((float)gfColorCurveTimes[(index + 1) / 4][(index + 1) % 4] - (float)gfColorCurveTimes[index / 4][index % 4]);


		float4 EmissiveColor;
		EmissiveColor.r = lerp(gfR_CurvePoints[index / 4][index % 4], gfR_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
		EmissiveColor.g = lerp(gfG_CurvePoints[index / 4][index % 4], gfG_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);
		EmissiveColor.b = lerp(gfB_CurvePoints[index / 4][index % 4], gfB_CurvePoints[(index + 1) / 4][(index + 1) % 4], t);

		cColor.rgb *= EmissiveColor.rgb * input.emissive;
	}
	else
		cColor.rgb *= input.color * input.emissive; // 변경 필요
	//cColor = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
	//cColor.a *= gnTexturesMask * 0.01f; // 0~100으로 받아 0.00 ~1.00으로 변경
	//cColor.a *= input.alpha; // 사라지는 효과 구현 위해(위 아래로 내려가는 파티클을 위해)
	//cColor.a = 1.0f - (gfCurrentTime - input.EmitTime) / input.lifetime;
	if (input.ParticleType == 5 || input.ParticleType == 3)
		cColor.a *= 1.0f - (gfCurrentTime - input.EmitTime) / input.lifetime;

	output.f4Color = cColor;

	return(output);
}
