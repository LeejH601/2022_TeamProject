#include "LobbyScene.h"
#include "SimulatorScene.h"
#include "..\Global\Global.h"
#include "..\Global\Locator.h"
#include "..\ImGui\ImGuiManager.h"

void CLobbyScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CImGuiManager::GetInst()->SetUI();
	//CImGuiManager::GetInst()->DemoRendering();

	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CLobbyScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
}

bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

SCENE_RETURN_TYPE CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			return SCENE_RETURN_TYPE::POP_SCENE;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		default:
			break;
		}
	default:
		break;
	}
	return SCENE_RETURN_TYPE::NONE;
}

void CLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CSimulatorScene::GetInst()->BuildObjects(pd3dDevice, pd3dCommandList);
}

void CLobbyScene::Update(float fTimeElapsed)
{
	CSimulatorScene::GetInst()->Update(fTimeElapsed);
}

void CLobbyScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	CImGuiManager::GetInst()->Render(pd3dCommandList);
}

void CLobbyScene::OnPostRender()
{
	CSimulatorScene::GetInst()->OnPostRender();
}
