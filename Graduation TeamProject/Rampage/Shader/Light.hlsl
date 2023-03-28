//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			4 

#define FRAME_BUFFER_WIDTH		1920
#define FRAME_BUFFER_HEIGHT		1080

#define _DEPTH_BUFFER_WIDTH		(FRAME_BUFFER_WIDTH * 4)
#define _DEPTH_BUFFER_HEIGHT	(FRAME_BUFFER_HEIGHT * 4)

#define DELTA_X					(1.0f / _DEPTH_BUFFER_WIDTH)
#define DELTA_Y					(1.0f / _DEPTH_BUFFER_HEIGHT)

#define MAX_DEPTH_TEXTURES		MAX_LIGHTS

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
#define _WITH_PCF_FILTERING

struct LIGHT
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular;
	float3					m_vPosition;
	float 					m_fFalloff;
	float3					m_vDirection;
	float 					m_fTheta; //cos(m_fTheta)
	float3					m_vAttenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int 					m_nType;
	float					m_fRange;
	float					padding;
};
cbuffer cbLights : register(b4)
{
	LIGHT					gLights[MAX_LIGHTS];
	float4					gcGlobalAmbientLight;
	int						gnLights;
};

Texture2D<float> gtxtDepthTextures[MAX_DEPTH_TEXTURES] : register(t23);
SamplerComparisonState gssComparisonPCFShadow : register(s2);

float4 DirectionalLight(int nIndex, float4 fColor, float3 vNormal, float3 vToCamera)
{
	float3 vToLight = -gLights[nIndex].m_vDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);

	return(gLights[nIndex].m_cDiffuse * fDiffuseFactor * fColor);
}

float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}


float4 Lighting(float3 vPosition, float3 vNormal, float4 fColor)
{
	float3 gf3_CameraPosition = float3(gf3CameraPosition.x, gf3CameraPosition.y, gf3CameraPosition.z);
	float3 vToCamera = normalize(gf3_CameraPosition - vPosition);
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	[unroll(MAX_LIGHTS)] for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				cColor += DirectionalLight(i, fColor, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == POINT_LIGHT)
			{
				cColor += PointLight(i, vPosition, vNormal, vToCamera);
			}
			else if (gLights[i].m_nType == SPOT_LIGHT)
			{
				cColor += SpotLight(i, vPosition, vNormal, vToCamera);
			}
		}
	}

	cColor += gcGlobalAmbientLight * fColor;
	return(cColor);
}

#define PCF_BY_5X5
float Compute3x3ShadowFactor(float2 uv, float fDepth, uint nIndex)
{
	float fPercentLit = gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv, fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-DELTA_X, 0.0f), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+DELTA_X, 0.0f), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(0.0f, -DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(0.0f, +DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-DELTA_X, -DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-DELTA_X, +DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+DELTA_X, -DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+DELTA_X, +DELTA_Y), fDepth).r;

#ifdef PCF_BY_5X5
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-2.0f * DELTA_X, 0.0f), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+2.0f * DELTA_X, 0.0f), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(0.0f, -2.0f * DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(0.0f, +2.0f * DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-2.0f * DELTA_X, -2.0f * DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(-2.0f * DELTA_X, +2.0f * DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+2.0f * DELTA_X, -2.0f * DELTA_Y), fDepth).r;
	fPercentLit += gtxtDepthTextures[nIndex].SampleCmpLevelZero(gssComparisonPCFShadow, uv + float2(+2.0f * DELTA_X, +2.0f * DELTA_Y), fDepth).r;

	return(fPercentLit / 17.0f);
#else
	return(fPercentLit / 9.0f);
#endif

}

float4 Lighting(float3 vPosition, float3 vNormal, float4 fColor, bool bShadow, float4 uvs[MAX_LIGHTS])
{
	float3 gf3_CameraPosition = float3(gf3CameraPosition.x, gf3CameraPosition.y, gf3CameraPosition.z);
	float3 vToCamera = normalize(gf3_CameraPosition - vPosition);
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	[unroll(MAX_LIGHTS)] for (int i = 0; i < gnLights; i++)
	{
		if (gLights[i].m_bEnable)
		{
			float fShadowFactor = 0.0f;
#ifdef _WITH_PCF_FILTERING
			if (bShadow) fShadowFactor = Compute3x3ShadowFactor(uvs[i].xy / uvs[i].ww, uvs[i].z / uvs[i].w, i);
#else
			if (bShadow) fShadowFactor = gtxtDepthTextures[i].SampleCmpLevelZero(gssComparisonPCFShadow, uvs[i].xy / uvs[i].ww, uvs[i].z / uvs[i].w).r;
			fShadowFactor = max(0.01f, fShadowFactor);
#endif
			cColor += gLights[i].m_cAmbient * fColor;
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				cColor += DirectionalLight(i, fColor, vNormal, vToCamera);
				//cColor += DirectionalLight(i, fColor, vNormal, vToCamera) * fShadowFactor;
				//cColor.rgb += DirectionalLight(i, fColor, vNormal, vToCamera).rgb * fShadowFactor;
			}
			else if (gLights[i].m_nType == POINT_LIGHT)
			{
				cColor += lerp(PointLight(i, fColor, vNormal, vToCamera), float4(0.f, 0.f, 0.f, 1.f), 1.0f - fShadowFactor);
				//cColor += PointLight(i, vPosition, vNormal, vToCamera) * fShadowFactor;
			}
			else if (gLights[i].m_nType == SPOT_LIGHT)
			{
				cColor += SpotLight(i, vPosition, vNormal, vToCamera) * fShadowFactor;
			}
			/*float4 ShadowColor = lerp(float4(0.f, 0.f, 0.f, 1.f), cColor, fShadowFactor);
			cColor = lerp(cColor, ShadowColor, 1.0f - fShadowFactor);*/
			cColor.rgb = cColor.rgb * fShadowFactor;
		}
	}

	cColor += gcGlobalAmbientLight * fColor;
	return(cColor);
}
