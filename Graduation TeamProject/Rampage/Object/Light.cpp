#include "Light.h"
#include "Player.h"

CLight::CLight()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.45f, 0.45f, 0.45f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_fRange = 500.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(-(_PLANE_WIDTH * 0.5f), 512.0f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 120.0f, -100.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(+1.0f, -1.0f, 0.0f);

	m_pLights[1].m_bEnable = false;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 25000.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.54f, 0.54f, 0.54f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.13f, 0.13f, 0.13f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-250.0f, 250.0f, 0.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(+1.0f, -1.0f, 0.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.1f, 0.001f);
	m_pLights[1].m_fFalloff = 16.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(60.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights[2].m_bEnable = false;
	m_pLights[3].m_bEnable = false;
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

void CLight::Update(CPlayer* pPlayer)
{
	/*if (m_pLights && m_pLights[0].m_bEnable)
	{
		XMFLOAT3 xmf3Position = pPlayer->GetPosition();

		m_pLights[0].m_xmf3Position = XMFLOAT3{ xmf3Position.x, xmf3Position.y + 128.0f, xmf3Position.z };
	}*/
}
