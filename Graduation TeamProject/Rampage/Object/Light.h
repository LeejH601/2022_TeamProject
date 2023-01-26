#pragma once
#include "..\Global\stdafx.h"

struct LIGHT
{
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
struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};
class CLight
{
public:
	CLight();
	virtual ~CLight();

	LIGHT* GetLights() { return m_pLights; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateLightVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseLightVariables();
private:
	XMFLOAT4 m_xmf4GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.5f);

	LIGHT* m_pLights = NULL;
	int	m_nLights = 0;

	ComPtr<ID3D12Resource> m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
};
