#pragma once
#include "..\Global\stdafx.h"

struct LIGHTS
{
	XMFLOAT4				m_xmf4GlobalAmbient;
};

class CLight
{

public:
	CLight();
	virtual ~CLight();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateLightVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseLightVariables();
private:
	XMFLOAT4 m_xmf4GlobalAmbient = XMFLOAT4(1.55f, 1.55f, 1.55f, 1.0f);

	ComPtr<ID3D12Resource> m_pd3dcbLights = NULL;
	LIGHTS*		   m_pcbMappedLights = NULL;
};