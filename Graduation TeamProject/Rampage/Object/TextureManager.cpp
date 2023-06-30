#include "TextureManager.h"
#include "..\Shader\BillBoardObjectShader.h"
#include "..\Shader\ParticleShader.h"

void CTextureManager::CreateCbvSrvUavDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews, int nUnorderedAccessViews)
{
	m_pd3dSrvCpuDescriptorHandles.resize(nShaderResourceViews);
	m_pd3dSrvGpuDescriptorHandles.resize(nShaderResourceViews);
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews + nUnorderedAccessViews;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dUavCPUDescriptorStartHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nShaderResourceViews);
	m_d3dUavGPUDescriptorStartHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nShaderResourceViews);

	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
	m_d3dUavCPUDescriptorNextHandle = m_d3dUavCPUDescriptorStartHandle;
	m_d3dUavGPUDescriptorNextHandle = m_d3dUavGPUDescriptorStartHandle;
}

void CTextureManager::CreateResourceView(ID3D12Device* pd3dDevice, UINT nDescriptorHeapIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	for (int i = 0; i < (UINT)TextureType::TextureType_End; i++)
	{
		for (int j = 0; j < m_iTextureN[i]; j++)
		{
			ID3D12Resource* pShaderResource = m_pTextures[i]->GetResource(j);
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = m_pTextures[i]->GetShaderResourceViewDesc(j);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			m_pd3dSrvCpuDescriptorHandles[i] = m_d3dSrvCPUDescriptorNextHandle;
			m_pd3dSrvGpuDescriptorHandles[i] = m_d3dSrvGPUDescriptorNextHandle;
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
}

void CTextureManager::SetTextureDescriptorHeap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dCbvSrvUavDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());
}

void CTextureManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(10, m_d3dCbvGPUDescriptorStartHandle);
}

void CTextureManager::LoadSphereBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1024];
	for (int i = 0; i < 1024; i++)
	{
		pxmf4RandomValues[i].x = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].y = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].z = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].w = float((rand() % 10000) - 5000) / 5000.0f;
	}

	m_pTextures[(UINT)TextureType::SphereTexture]->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1024, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, m_iTextureN[(UINT(TextureType::SphereTexture))]++/*, RESOURCE_BUFFER*/);
}

void CTextureManager::LoadTexture(TextureType eTextureType, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column)
{
	// Texture ������ Ȯ��
	for (int i = 0; i < m_iTextureN[(UINT(eTextureType))]; i++)
	{
		if (!wcscmp(m_pTextures[UINT(eTextureType)]->GetTextureName(i), pszFileName))
		{
			return;
		}
	}
	m_pTextures[UINT(eTextureType)]->SetRowColumn(m_iTextureN[(UINT(eTextureType))], iRow, Column);
	m_pTextures[UINT(eTextureType)]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, m_iTextureN[(UINT(eTextureType))]++);
}

int CTextureManager::LoadTextureIndex(TextureType eTextureType, LPCTSTR pszFileName)
{
	for (int i = 0; i < m_iTextureN[(UINT(eTextureType))]; i++)
	{
		if (!wcscmp(m_pTextures[(UINT(eTextureType))]->GetTextureName(i), pszFileName))
		{
			return i;
		}
	}
	return -1;
}
int CTextureManager::LoadTotalTextureIndex(TextureType eTextureType, LPCTSTR pszFileName)
{
	int iTextureOffSet = GetTextureOffset(eTextureType); // 텍스쳐별로 Offset
	int iTypeTextureIndex = LoadTextureIndex(eTextureType, pszFileName); // Type별로 텍스쳐 인덱스
	if (iTypeTextureIndex == -1) // 텍스쳐X
		return iTypeTextureIndex;
	
	return iTextureOffSet + iTypeTextureIndex;
}
std::shared_ptr<CTexture> CTextureManager::GetTexture(TextureType eTextureType)
{
	return m_pTextures[(UINT(eTextureType))];
}

UINT CTextureManager::GetTextureListIndex(TextureType eTextureType)
{
	return m_iTextureN[(UINT(eTextureType))];
}

UINT CTextureManager::GetTextureOffset(TextureType eTextureType)
{
	int iOffset = 0;
	for (int i = 1; i < UINT(eTextureType); i++) // Sphere Buffer는 제외 Shader t50 사용
		iOffset += m_iTextureN[i];

	return iOffset;
}
