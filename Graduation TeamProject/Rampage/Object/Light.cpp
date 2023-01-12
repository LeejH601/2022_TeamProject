#include "Light.h"


void CLightManager::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateShaderVariables(pd3dCommandList);
}

void CLightManager::CreateLightVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights.Get()->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CLightManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int LightN = vLights.size();
	if(LightN > 0)
		::memcpy(m_pcbMappedLights->m_pLights, &vLights[0], sizeof(LIGHT) * LightN);

	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &LightN, sizeof(int));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights.Get()->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATUREINDEX_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights
}

void CLightManager::ReleaseLightVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CLightManager::Add_Light(LIGHT sLight)
{
	vLights.emplace_back(sLight);
}
