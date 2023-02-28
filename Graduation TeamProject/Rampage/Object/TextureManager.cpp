#include "TextureManager.h"
#include "..\Shader\BillBoardObjectShader.h"

std::shared_ptr<CTexture> CTextureManager::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, CBillBoardObjectShader* pShader, int iRow, int Column)
{
	for (const auto& texture : m_vTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}

	std::shared_ptr<CTexture> pSpriteTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	pSpriteTexture->SetRowColumn(iRow, Column);
	pSpriteTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	pShader->CreateShaderResourceViews(pd3dDevice, pSpriteTexture.get(), 0, 10);

	m_vTextures.emplace_back(pSpriteTexture);

	return pSpriteTexture;
}

std::shared_ptr<CTexture> CTextureManager::LoadTexture(LPCTSTR pszFileName)
{
	for (const auto& texture : m_vTextures)
	{
		if (!wcscmp(texture->GetTextureName(0), pszFileName))
			return texture;
	}
	return nullptr;
}

std::vector<std::shared_ptr<CTexture>>& CTextureManager::GetTextureList()
{
	return m_vTextures;
}
