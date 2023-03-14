#include "..\Global\Camera.h"
#include "SceneManager.h"
#include "MainScene.h"
#include "SimulatorScene.h"
#include "LobbyScene.h"
#include "..\Global\Locator.h"

CSceneManager::CSceneManager()
{
}

CSceneManager::CSceneManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pMainScene = std::make_unique<CMainTMPScene>();
	m_pMainScene->BuildObjects(pd3dDevice, pd3dCommandList);

	m_pLobbyScene = std::make_unique<CLobbyScene>();
	m_pLobbyScene->BuildObjects(pd3dDevice, pd3dCommandList);

	m_pCurrentScene = m_pLobbyScene.get();
	//m_pCurrentScene = m_pMainScene.get();
}

void CSceneManager::SetPlayer(CPlayer* pPlayer)
{
	if (m_pLobbyScene)
		m_pLobbyScene->SetPlayer(pPlayer);
	if (m_pMainScene)
		m_pMainScene->SetPlayer(pPlayer);
}

void CSceneManager::SetCurrentScene(SCENE_TYPE scene_type)
{
	switch (scene_type)
	{
	case SCENE_TYPE::LOBBY_SCENE:
		m_pCurrentScene = m_pLobbyScene.get();
		break;
	case SCENE_TYPE::MAIN_SCENE:
		m_pCurrentScene = m_pMainScene.get();
		break;
	default:
		break;
	}
}

void CSceneManager::PreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	m_pCurrentScene->OnPrepareRender(pd3dCommandList);
	m_pCurrentScene->OnPreRender(pd3dCommandList, fTimeElapsed);
}

void CSceneManager::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	m_pCurrentScene->OnPrepareRenderTarget(pd3dCommandList, nRenderTargets, pd3dRtvCPUHandles, d3dDepthStencilBufferDSVCPUHandle);
}

void CSceneManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	m_pCurrentScene->Render(pd3dCommandList, fTimeElapsed, fCurrentTime, pCamera);
}

void CSceneManager::OnPostRenderTarget()
{
	m_pCurrentScene->OnPostRenderTarget();
}

void CSceneManager::Animate(float fTimeElapsed)
{
	m_pCurrentScene->AnimateObjects(fTimeElapsed);
}

bool CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	return 0;
}

bool CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	bool bChange = m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, dwDirection);

	if (bChange)
		m_pCurrentScene = m_pMainScene.get();

	return 0;
}
