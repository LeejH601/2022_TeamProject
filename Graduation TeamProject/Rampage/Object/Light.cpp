#include "Light.h"


CLight::CLight()
{
	m_nLights = 1;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 500.f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(1.f, 0.f, 0.f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(0.F, 30.0f, 4.F);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.00001f, 0.0001f);
}

CLight::~CLight()
{
}

void CLight::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATUREINDEX_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights
}

void CLight::CreateLightVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}


void CLight::ReleaseLightVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}
