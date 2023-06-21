#pragma once
#include "Scene.h"
#include "../Object/TextureManager.h"
enum class LobbySceneType
{
	LOGO_Scene,
	SIMULATOR_Scene,
	Main_Scene
};
class CLobbyScene : public CScene
{
private:
	std::unique_ptr<CShader> m_pUIObjectShader;
	std::vector<std::unique_ptr<CGameObject>> m_pUIObject;

	std::unique_ptr<CTextureManager> m_pTextureManager = NULL;
	CCamera* m_pCamera = NULL;

	LobbySceneType m_iSceneType = LobbySceneType::LOGO_Scene;
public:
	DECLARE_SINGLE(CLobbyScene);
	CLobbyScene() {}
	virtual ~CLobbyScene() {}

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual SCENE_RETURN_TYPE OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Update(float fTimeElapsed);
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	virtual void OnPostRender();

	virtual bool ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed);

	UINT GetSceneType() { return (UINT)m_iSceneType; };
	void  SetSceneType(UINT iSceneType) { m_iSceneType = (LobbySceneType)iSceneType; };
};