#pragma once
#include "..\Global\Global.h"
#include "..\Global\stdafx.h"

class CPlayer;
class CCamera;
class CScene;
class CSceneManager {
private:
	CPlayer* m_pPlayer = NULL;
	CScene* m_pCurrentScene = NULL;
	SCENE_TYPE m_CurrentScene;
	std::unique_ptr<CScene> m_pMainScene = NULL;
	std::unique_ptr<CScene> m_pLobbyScene = NULL;
	std::unique_ptr<CScene> m_pSimulatorScene = NULL;
public:
	CSceneManager();
	CSceneManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CSceneManager() { }

	void SetPlayer(CPlayer* pPlayer);
	void SetCurrentScene(SCENE_TYPE scene_type);
	SCENE_TYPE GetCurrentScene() { return m_CurrentScene; }
	void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	void OnPostRender();
	void Update(float fTimeElapsed);

	bool ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);
};