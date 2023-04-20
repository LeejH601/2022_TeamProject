#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\Component.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\State.h"

class DataLoader
{
	std::wstring file_path = L"Data\\";
	std::wstring file_ext = L".bin";
public:
	void SaveComponentSets(std::wstring wFolderName);
	void LoadComponentSets(std::wstring wFolderName);

	void SaveComponentSet(FILE* pInFile, CState<CPlayer>* pState);
	void LoadComponentSet(FILE* pInFile, CState<CPlayer>* pState);
};

class CTexture;
class CCamera;
class CImGuiManager
{
private:
	// Component Sets
	std::unique_ptr<DataLoader> m_pDataLoader = NULL;

	// Handle Window
	HWND m_hWnd;

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);;
	float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	
	long m_lDesktopWidth = 0.0f;
	long m_lDesktopHeight = 0.0f;

	float ParallaxScale = 0.0f;
	float ParallaxBias = 0.0f;
	int Terrain_Mapping_mode = 0;

	int Player_Animation_Number = 0;

	bool show_simulator_scene = false;
	bool show_preset_menu = false;
	bool show_save_menu = false;

	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_my_window = true;

	CCamera* m_pCamera = NULL;
	ImVec2 dearImGuiSize;

	std::unique_ptr<CTexture> m_pRTTexture;
	D3D12_CPU_DESCRIPTOR_HANDLE m_pd3dRtvCPUDescriptorHandles;

	ComPtr<ID3D12DescriptorHeap> m_pd3dSrvDescHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;
public:
	DECLARE_SINGLE(CImGuiManager);

	CImGuiManager();
	~CImGuiManager();
	void CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice);
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUDescriptorHandle() {
		return m_pd3dRtvCPUDescriptorHandles;
	}
	ID3D12Resource* GetRTTextureResource(); 

	void Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, const RECT& DeskTopCoordinatesRect);

	void DemoRendering();
	void SetUI();

	void ShowImpactManager(CState<CPlayer>* pCurrentAnimation);
	void ShowParticleManager(CState<CPlayer>* pCurrentAnimation);
	void ShowDamageAnimationManager(CState<CPlayer>* pCurrentAnimation);
	void ShowShakeAnimationManager(CState<CPlayer>* pCurrentAnimation);
	void ShowStunAnimationManager(CState<CPlayer>* pCurrentAnimation);
	void ShowHitLagManager(CState<CPlayer>* pCurrentAnimation);
	void ShowCameraMoveManager(CState<CPlayer>* pCurrentAnimation);
	void ShowCameraShakeManager(CState<CPlayer>* pCurrentAnimation);
	void ShowCameraZoomManager(CState<CPlayer>* pCurrentAnimation);
	void ShowShockSoundManager(CState<CPlayer>* pCurrentAnimation);
	void ShowShootSoundManager(CState<CPlayer>* pCurrentAnimation);
	void ShowDamageMoanSoundManager(CState<CPlayer>* pCurrentAnimation);

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	void PrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPostRender();
	void OnDestroy();
	void OnPostRenderTarget();
	D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvDescHandle() { return &m_pd3dRtvCPUDescriptorHandles; };

	ImVec4 GetColor() { return clear_color; };
	int GetAnimationNum() { return Player_Animation_Number; };
	float GetParallaxScale() { return ParallaxScale; };
	float GetParallaxBias() { return ParallaxBias; };
	int GetTerrainMappingMode() { return Terrain_Mapping_mode; };
};