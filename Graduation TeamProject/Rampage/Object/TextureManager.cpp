#include "TextureManager.h"
#include "..\Shader\BillBoardObjectShader.h"
#include "..\Shader\ParticleShader.h"
#include "..\Global\Utility.h"

std::shared_ptr<CTexture> CTextureManager::LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CBillBoardObjectShader* pShader, int iRow, int Column)
{
	for (const auto& texture : m_vBillBoardTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}

	std::shared_ptr<CTexture> pSpriteTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	pSpriteTexture->SetRowColumn(iRow, Column);
	pSpriteTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	pShader->CreateShaderResourceViews(pd3dDevice, pSpriteTexture.get(), 0, 10);

	m_vBillBoardTextures.emplace_back(pSpriteTexture);

	return pSpriteTexture;
}

std::shared_ptr<CTexture> CTextureManager::SetPerlinNoiseTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CShader* pShader)
{
	std::shared_ptr<CTexture> pNoiseTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	std::vector<XMFLOAT4> data; GeneratePerlinNoise(128, 128, 1.0, 1, 1.0f, 1.0f, data);
	pNoiseTexture->CreateTexture(pd3dDevice, pd3dCommandList, 128, 128,1,1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, L"testNoise", RESOURCE_TEXTURE2D,0, (void*)(data.data()));

	pShader->CreateShaderResourceViews(pd3dDevice, pNoiseTexture.get(), 0, 10);

	m_vBillBoardTextures.emplace_back(pNoiseTexture);

	return pNoiseTexture;
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
