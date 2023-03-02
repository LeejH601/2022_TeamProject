#include "LobbyScene.h"
#include "SimulatorScene.h"
#include "..\ImGui\ImGuiManager.h"

void CLobbyScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CImGuiManager::GetInst()->SetUI();

	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CLobbyScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
}

bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			return 1;
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
	return 0;
}

void CLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CSimulatorScene::GetInst()->BuildObjects(pd3dDevice, pd3dCommandList);
}

void CLobbyScene::AnimateObjects(float fTimeElapsed)
{
	CSimulatorScene::GetInst()->AnimateObjects(fTimeElapsed);
}

void CLobbyScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	CImGuiManager::GetInst()->Render(pd3dCommandList);
}

void CLobbyScene::OnPostRenderTarget()
{
	CSimulatorScene::GetInst()->OnPostRenderTarget();
}
