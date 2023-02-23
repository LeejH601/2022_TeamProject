#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Texture.h"

class CTextureManager
{
public:
	DECLARE_SINGLE(CTextureManager);
	CTextureManager() {}
	~CTextureManager() {}
	std::shared_ptr<CTexture> LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, class CBillBoardObjectShader* pShader, int iRow, int Column);
	std::shared_ptr<CTexture> LoadTexture(LPCTSTR pszFileName);

	std::vector<std::shared_ptr<CTexture>>& GetTextureList();

protected:
	std::vector<std::shared_ptr<CTexture>> m_vTextures;
};