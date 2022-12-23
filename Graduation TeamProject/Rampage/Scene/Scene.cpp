#include "Scene.h"

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
}

void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pCamera = std::make_unique<CCamera>();
}

void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}
