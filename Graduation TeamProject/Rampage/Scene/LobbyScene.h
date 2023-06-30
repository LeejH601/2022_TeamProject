#pragma once
#include "Scene.h"
#include "../Object/TextureManager.h"
#include "..\Object\Light.h"
#include "..\Object\Map.h"
#include "..\Shader\SkyBoxShader.h"
#include "..\Object\SkyBox.h"
#include "..\Shader\SunLightShader.h"
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

	std::unique_ptr<CCamera> m_pCamera = NULL;

	std::unique_ptr<CMap> m_pMap;

	std::unique_ptr<CLight> m_pLight;

	std::unique_ptr<CSkyBoxShader> m_pSkyBoxShader;
	std::unique_ptr<CSkyBox> m_pSkyBoxObject;

	std::unique_ptr<CSunLightShader> m_pSunLightShader;

	LobbySceneType m_iSceneType = LobbySceneType::LOGO_Scene;
	POINT m_ptOldCursorPos;
	RECT m_ScreendRect;
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