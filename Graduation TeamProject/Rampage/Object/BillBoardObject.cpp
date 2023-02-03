#include "../Object/Texture.h"
#include "BillBoardObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"

CBillBoardObject::CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CBillBoardObjectShader* pShader)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	//std::shared_ptr<CTexture> pSpriteTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);

	//pSpriteTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pFileName, RESOURCE_TEXTURE2D, 0);

	pShader->CreateShaderResourceViews(pd3dDevice, pSpriteTexture.get(), 0, 8);

	SetTexture(pSpriteTexture);

	SetMesh(std::make_shared< CTexturedRectMesh>(pd3dDevice, pd3dCommandList, 8.0f, 8.0f, 0.0f, 0.0f, 0.0f, 0.0f));
}

CBillBoardObject::~CBillBoardObject()
{
}

void CBillBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
	CGameObject::Render(pd3dCommandList, pCamera);
}

