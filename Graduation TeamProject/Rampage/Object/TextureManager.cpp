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

void CTextureManager::CreateResourceView(ID3D12Device* pd3dDevice, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndexBillBoard, UINT nRootParameterStartIndexParticle)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	for (int i = 0; i < m_iBillBoardTextureN; i++)
	{
		ID3D12Resource* pShaderResource = m_pTextures->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = m_pTextures->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		//pTexture->m_pd3dSrvCpuDescriptorHandles[i] = m_d3dSrvCPUDescriptorNextHandle;
		//pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_pd3dSrvCpuDescriptorHandles[i] = m_d3dSrvCPUDescriptorNextHandle;
		m_pd3dSrvGpuDescriptorHandles[i] = m_d3dSrvGPUDescriptorNextHandle;
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	for (int i = m_iBillBoardTextureN; i < m_iBillBoardTextureN + m_iParticleTextureN; i++)
	{
		ID3D12Resource* pShaderResource = m_pTextures->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = m_pTextures->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_pd3dSrvCpuDescriptorHandles[i] = m_d3dSrvCPUDescriptorNextHandle;
		m_pd3dSrvGpuDescriptorHandles[i] = m_d3dSrvGPUDescriptorNextHandle; 
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}


}

void CTextureManager::SetTextureDescriptorHeap(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dCbvSrvUavDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvUavDescriptorHeap.GetAddressOf());
}

void CTextureManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//if (m_pd3dSrvGpuDescriptorHandles[0].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(10, m_pd3dSrvGpuDescriptorHandles[0]);
	//for (int i = 0; i < m_iBillBoardTextureN; i++)
	//{
	//	if (m_pd3dSrvGpuDescriptorHandles[i].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(10, m_pd3dSrvGpuDescriptorHandles[i]);
	//}

	//for (int i = m_iBillBoardTextureN; i < m_iBillBoardTextureN + m_iParticleTextureN; i++)
	//{
	//	if (m_pd3dSrvGpuDescriptorHandles[i].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(12 + (i - m_iBillBoardTextureN) % 3, m_pd3dSrvGpuDescriptorHandles[i]);
	//}
	pd3dCommandList->SetGraphicsRootDescriptorTable(10, m_d3dCbvGPUDescriptorStartHandle);
}

void CTextureManager::LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column)
{
	for (int i = 0; i < m_iBillBoardTextureN; i++)
	{
		if (!wcscmp(m_pTextures->GetTextureName(i), pszFileName))
		{
			return;
		}
	}
	m_pTextures->SetRowColumn(m_iBillBoardTextureN, iRow, Column);
	m_pTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, m_iBillBoardTextureN);
	m_iBillBoardTextureN++;
}
void CTextureManager::LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, int iRow, int Column)
{
	for (int i = m_iBillBoardTextureN; i < m_iBillBoardTextureN + m_iParticleTextureN; i++)
	{
		if (!wcscmp(m_pTextures->GetTextureName(i), pszFileName))
		{
			return;
		}
	}
	m_pTextures->SetRowColumn(m_iBillBoardTextureN + m_iParticleTextureN, iRow, Column);
	m_pTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, m_iBillBoardTextureN + m_iParticleTextureN);
	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1024];
	for (int i = 0; i < 1024; i++)
	{
		pxmf4RandomValues[i].x = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].y = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].z = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].w = float((rand() % 10000) - 5000) / 5000.0f;
	}
	m_pTextures->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1024, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, m_iBillBoardTextureN + m_iParticleTextureN + 1/*, RESOURCE_BUFFER*/);
	m_pTextures->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 256, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, m_iBillBoardTextureN + m_iParticleTextureN + 2/*, RESOURCE_TEXTURE1D*/);
	m_iParticleTextureN += 3;
}

//void CTextureManager::LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CParticleShader* pShader, int iRow, int Column, bool i)
//{
//	for (const auto& texture : m_vParticleTextures)
//	{
//		if (!wcscmp(texture->GetTextureName(0), pszFileName))
//			return texture;
//	}
//
//	std::shared_ptr<CTexture> pParticleTexture = std::make_shared<CTexture>(3, RESOURCE_TEXTURE1D, 0, 1);
//	pParticleTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);
//
//	srand((unsigned)time(NULL));
//
//	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1024];
//	for (int i = 0; i < 1024; i++)
//	{
//		pxmf4RandomValues[i].x = float((rand() % 10000) - 5000) / 5000.0f;
//		pxmf4RandomValues[i].y = float((rand() % 10000) - 5000) / 5000.0f;
//		pxmf4RandomValues[i].z = float((rand() % 10000) - 5000) / 5000.0f;
//		pxmf4RandomValues[i].w = float((rand() % 10000) - 5000) / 5000.0f;
//	}
//
//	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1024, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 1/*, RESOURCE_BUFFER*/);
//	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 256, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 2/*, RESOURCE_TEXTURE1D*/);
//
//	pShader->CreateShaderResourceViews(pd3dDevice, pParticleTexture.get(), 0, 12);
//
//	m_vParticleTextures.emplace_back(pParticleTexture);
//
//	return pParticleTexture;
//}
std::shared_ptr<CTexture> CTextureManager::LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CBillBoardObjectShader* pShader, int iRow, int Column)
{
	for (const auto& texture : m_vBillBoardTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}

	std::shared_ptr<CTexture> pSpriteTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	pSpriteTexture->SetRowColumn(0, iRow, Column);
	pSpriteTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	pShader->CreateShaderResourceViews(pd3dDevice, pSpriteTexture.get(), 0, 10);

	m_vBillBoardTextures.emplace_back(pSpriteTexture);

	return pSpriteTexture;
}

std::shared_ptr<CTexture> CTextureManager::LoadBillBoardTexture(LPCTSTR pszFileName)
{
	for (const auto& texture : m_vBillBoardTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}
	return nullptr;
}

std::vector<std::shared_ptr<CTexture>>& CTextureManager::GetBillBoardTextureList()
{
	return m_vBillBoardTextures;
}

std::shared_ptr<CTexture> CTextureManager::LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CParticleShader* pShader, int iRow, int Column)
{
	for (const auto& texture : m_vParticleTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}

	std::shared_ptr<CTexture> pParticleTexture = std::make_shared<CTexture>(3, RESOURCE_TEXTURE1D, 0, 1);
	pParticleTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	srand((unsigned)time(NULL));

	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1024];
	for (int i = 0; i < 1024; i++)
	{
		pxmf4RandomValues[i].x = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].y = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].z = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].w = float((rand() % 10000) - 5000) / 5000.0f;
	}

	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1024, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 1/*, RESOURCE_BUFFER*/);
	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 256, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 2/*, RESOURCE_TEXTURE1D*/);

	pShader->CreateShaderResourceViews(pd3dDevice, pParticleTexture.get(), 0, 12);

	m_vParticleTextures.emplace_back(pParticleTexture);

	return pParticleTexture;
}

std::shared_ptr<CTexture> CTextureManager::LoadParticleTexture(LPCTSTR pszFileName)
{
	for (const auto& texture : m_vParticleTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}
	return nullptr;


}

std::vector<std::shared_ptr<CTexture>>& CTextureManager::GetParticleTextureList()
{
	return m_vParticleTextures;
}
