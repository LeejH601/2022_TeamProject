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
	std::unique_ptr<CScene> m_pMainScene = NULL;
	std::unique_ptr<CScene> m_pLobbyScene = NULL;
	std::unique_ptr<CScene> m_pSimulatorScene = NULL;
	
public:
	CSceneManager();
	CSceneManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CSceneManager() { }

	void SetPlayer(CPlayer* pPlayer);
	void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	void OnPostRender();
	void Update(float fTimeElapsed);

	bool ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);

	CScene* GetCurrentScene() { return m_pCurrentScene; };
	CScene* GetMainScene() { 
		return m_pMainScene.get(); 
	};
};