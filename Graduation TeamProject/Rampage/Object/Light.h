#pragma once
#include "..\Global\stdafx.h"



enum LIGHT_TYPE
{
	POINT_LIGHT = 1,
	SPOT_LIGHT,
	DIRECTIONAL_LIGHT,
	LIGHT_TYPE_END
};

struct LIGHT
{
	LIGHT() {};
	// DIRECTIONAL_LIGHT
	LIGHT(LIGHT_TYPE eType, float fRange, XMFLOAT4 xmf4Ambient, XMFLOAT4 xmf4Diffuse, XMFLOAT4 xmf4Specular, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Directional, bool bEnable = true)
	{
		m_nType = eType, m_fRange = fRange, m_xmf4Ambient = xmf4Ambient, m_xmf4Diffuse = xmf4Diffuse, m_xmf4Specular = xmf4Specular, m_xmf3Position = xmf3Position, m_xmf3Direction = xmf3Directional, m_bEnable = bEnable;
	};
	// SPOT_LIGHT
	LIGHT(LIGHT_TYPE eType, float fRange, XMFLOAT4 xmf4Ambient, XMFLOAT4 xmf4Diffuse, XMFLOAT4 xmf4Specular, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Directional, XMFLOAT3 xmf3Attenuation, float fFalloff, float fPhi, float fTheta, bool bEnable = true)
	{
		m_nType = eType, m_fRange = fRange, m_xmf4Ambient = xmf4Ambient, m_xmf4Diffuse = xmf4Diffuse, m_xmf4Specular = xmf4Specular, m_xmf3Position = xmf3Position, m_xmf3Direction = xmf3Directional, m_xmf3Attenuation = xmf3Attenuation, m_fFalloff = fFalloff, m_fPhi = fPhi, m_fTheta = fTheta, m_bEnable = bEnable;
	};
	// POINT_LIGHT
	LIGHT(LIGHT_TYPE eType, float fRange, XMFLOAT4 xmf4Ambient, XMFLOAT4 xmf4Diffuse, XMFLOAT4 xmf4Specular, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Directional, XMFLOAT3 xmf3Attenuation, bool bEnable = true)
	{
		m_nType = eType, m_fRange = fRange, m_xmf4Ambient = xmf4Ambient, m_xmf4Diffuse = xmf4Diffuse, m_xmf4Specular = xmf4Specular, m_xmf3Position = xmf3Position, m_xmf3Direction = xmf3Directional, m_xmf3Attenuation = xmf3Attenuation,  m_bEnable = bEnable;
	};

	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct TOOBJECTSPACEINFO
{
	XMFLOAT4X4						m_xmf4x4ToTexture;
	XMFLOAT4						m_xmf4Position;
};

struct TOLIGHTSPACES
{
	TOOBJECTSPACEINFO				m_pToLightSpaces[MAX_LIGHTS];
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
	TOOBJECTSPACEINFO		m_pToLightSpaces[MAX_LIGHTS];
};


class CLightManager
{

public:
	DECLARE_SINGLE(CLightManager);
	CLightManager() {};
	~CLightManager() {};

public:
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateLightVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseLightVariables();

public:
	void Add_Light(LIGHT sLight);
	LIGHT Get_Light(int index);
	int	Get_LightSize();
	
public:
	void Set_LightSpace(TOLIGHTSPACES& ToLightSpaces);

private:
	std::vector<LIGHT> vLights;

	XMFLOAT4 m_xmf4GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.5f);

	TOLIGHTSPACES m_pToLightSpaces;

	ComPtr<ID3D12Resource> m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
};
