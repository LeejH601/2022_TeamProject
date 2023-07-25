#include "..\Global\Camera.h"
#include "SceneManager.h"
#include "MainScene.h"
#include "SimulatorScene.h"
#include "LobbyScene.h"
#include "..\Global\Locator.h"
#include "..\Sound\SoundManager.h"
#include "..\ImGui\ImGuiManager.h"

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

void CSceneManager::OnPostRender()
{
	m_pCurrentScene->OnPostRender();
}

void CSceneManager::Update(float fTimeElapsed)
{
	m_pCurrentScene->Update(fTimeElapsed);
}

bool CSceneManager::ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed)
{
	m_pCurrentScene->ProcessInput(hWnd, dwDirection, fTimeElapsed);

	return false;
}

SCENE_STATE CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	if (dynamic_cast<CLobbyScene*>(m_pCurrentScene))
	{
		if (dynamic_cast<CLobbyScene*>(m_pCurrentScene)->GetSceneType() == (UINT)LobbySceneType::Main_Scene)
		{
			dynamic_cast<CLobbyScene*>(m_pCurrentScene)->SetSceneType((UINT)LobbySceneType::SIMULATOR_Scene);
			m_pCurrentScene = m_pMainScene.get();
			m_pMainScene->Enter(hWnd);
			return SCENE_STATE::TURN_TO_MAINSCENE;
		}
	}

	return SCENE_STATE::NONE;
}

bool CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	SCENE_RETURN_TYPE scene_return_type = m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, dwDirection);


	switch (scene_return_type)
	{
	case SCENE_RETURN_TYPE::NONE:
		break;
	/*case SCENE_RETURN_TYPE::POP_LOGOSCENE:
		if (m_pCurrentScene == m_pLobbyScene.get()) {
			if (dynamic_cast<CLobbyScene*>(m_pCurrentScene))
				dynamic_cast<CLobbyScene*>(m_pCurrentScene)->SetSceneType((UINT)(LobbySceneType::SIMULATOR_Scene));
		}
		break;
	case SCENE_RETURN_TYPE::POP_SIMULATORSCENE:
		if (m_pCurrentScene == m_pLobbyScene.get()) {
			m_pMainScene->Enter(hWnd);
			m_pCurrentScene = m_pMainScene.get();
		}
		break;*/
	case SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE:
		if (m_pCurrentScene == m_pMainScene.get())
		{
			dynamic_cast<CMainTMPScene*>(m_pMainScene.get())->AllBackgroundSoundStop();
			m_pCurrentScene = m_pLobbyScene.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
		}
		else
		{
			if (dynamic_cast<CLobbyScene*>(m_pCurrentScene)->GetSceneType() == (UINT)LobbySceneType::SIMULATOR_Scene)
				dynamic_cast<CLobbyScene*>(m_pCurrentScene)->SetSceneType((UINT)LobbySceneType::LOGO_Scene);
		}
		break;
	case SCENE_RETURN_TYPE::SWITCH_LOGOSCENE:
		if (m_pCurrentScene == m_pMainScene.get())
		{
			dynamic_cast<CMainTMPScene*>(m_pMainScene.get())->AllBackgroundSoundStop();
			m_pCurrentScene = m_pLobbyScene.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
			dynamic_cast<CLobbyScene*>(m_pCurrentScene)->SetSceneType((UINT)LobbySceneType::LOGO_Scene);
			CSoundManager::GetInst()->PlaySound("Sound/Background/Ambient 8.wav", 1.0f, 0.5f);
			CImGuiManager::GetInst()->GetChangeBeforeScene() = false;
		}
		break;
		
	default:
		break;
	}

	return 0;
}
