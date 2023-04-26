#pragma once
#include "..\Global\stdafx.h"
#include "..\Object\Texture.h"

class CTextureManager
{
public:
	CTextureManager() {}
	~CTextureManager() {}

	// BillBoardTextures
	std::shared_ptr<CTexture> LoadBillBoardTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, class CBillBoardObjectShader* pShader, int iRow, int Column);
	std::shared_ptr<CTexture> LoadBillBoardTexture(LPCTSTR pszFileName);

	std::vector<std::shared_ptr<CTexture>>& GetBillBoardTextureList();

	// ParticleTextures
	std::shared_ptr<CTexture> LoadParticleTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName, class CParticleShader* pShader, int iRow = 1, int Column = 1);
	std::shared_ptr<CTexture> LoadParticleTexture(LPCTSTR pszFileName);

	std::vector<std::shared_ptr<CTexture>>& GetParticleTextureList();
protected:
	std::vector<std::shared_ptr<CTexture>> m_vBillBoardTextures;
	std::vector<std::shared_ptr<CTexture>> m_vParticleTextures;
};