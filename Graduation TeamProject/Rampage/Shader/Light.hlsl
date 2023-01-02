//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			16 
#define MAX_MATERIALS		512 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

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

cbuffer cbLights : register(b2)
{
	float4					gcGlobalAmbientLight;
};


float4 Lighting(float3 vPosition, float3 vNormal)
{
	float3 gf3CameraPosition = float3(gf3CameraPosition.x, gf3CameraPosition.y, gf3CameraPosition.z);
	float3 vToCamera = normalize(gf3CameraPosition - vPosition);
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	cColor += gcGlobalAmbientLight;

	return(cColor);
}

