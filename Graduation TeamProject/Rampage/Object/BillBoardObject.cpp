#include "../Object/Texture.h"
#include "BillBoardObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"

CBillBoardObject::CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	pShader->CreateShaderResourceViews(pd3dDevice, pSpriteTexture.get(), 0, 8);

	SetTexture(pSpriteTexture);

	SetMesh(std::make_shared<CBillBoardMesh>(pd3dDevice, pd3dCommandList));
}

CBillBoardObject::~CBillBoardObject()
{
}

void CBillBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

void CBillBoardObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);

	//XMFLOAT4X4 xmf4x4Texture;
	//XMStoreFloat4x4(&xmf4x4Texture, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Texture)));
	//pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4Texture, 16);
}

CMultiSpriteObject::CMultiSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSpeed) : CBillBoardObject(pSpriteTexture, pd3dDevice, pd3dCommandList, pShader)
{
	m_nRows = nRows;
	m_nCols = nCols;
	m_fSpeed = fSpeed;
}

CMultiSpriteObject::~CMultiSpriteObject()
{
}

void CMultiSpriteObject::AnimateRowColumn(float fTimeElapsed)
{
	m_xmf4x4World._11 = 1.0f / float(m_nRows);
	m_xmf4x4World._22 = 1.0f / float(m_nCols);
	m_xmf4x4World._31 = float(m_nRow) / float(m_nRows);
	m_xmf4x4World._32 = float(m_nCol) / float(m_nCols);
 	if (fTimeElapsed == 0.0f)
	{
		if (++m_nCol == m_nCols) { 
			m_nRow++; 
			m_nCol = 0; 
		}
		if (m_nRow == m_nRows) 
			m_nRow = 0;
	}
}

void CMultiSpriteObject::Animate(float fTimeElapsed)
{
	m_fTime += fTimeElapsed * 0.5f;
	if (m_fTime >= m_fSpeed) 
		m_fTime = 0.0f;
	AnimateRowColumn(m_fTime);
}