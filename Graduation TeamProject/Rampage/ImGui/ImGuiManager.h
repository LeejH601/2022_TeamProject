#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\Component.h"
#include "..\Global\MessageDispatcher.h"

typedef std::pair<int, std::shared_ptr<CComponentSet>> CoptSetPair;

class Comp_ComponentSet
{
public:
	bool operator()(const CoptSetPair& lhs, const CoptSetPair& rhs) const {
		return lhs.first < rhs.first;
	}
};

class DataLoader
{
	std::string file_path = "Data\\Component";
	std::string file_ext = ".bin";

public:
	void SaveComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets);
	void LoadComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets);

	void SaveComponentSet(FILE* pInFile, CComponentSet* componentset);
	void LoadComponentSet(FILE* pInFile, CComponentSet* componentset);
};

class CTexture;
class CCamera;
class CImGuiManager
{
private:
	// Component Sets
	std::unique_ptr<DataLoader> m_pDataLoader = NULL;

	std::set<CoptSetPair, Comp_ComponentSet> m_sComponentSets;
	CoptSetPair dummy;

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);;
	float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	
	float ParallaxScale = 0.0f;
	float ParallaxBias = 0.0f;
	int Terrain_Mapping_mode = 0;

	int Player_Animation_Number;

	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_my_window = true;

	CComponentSet* m_pCurrentComponentSet = nullptr;

	CCamera* m_pCamera = NULL;

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

	void Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle);

	void DemoRendering();
	void SetUI();
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL);
	void PrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPostRenderTarget();
	void OnDestroy();

	ImVec4 GetColor() { return clear_color; };
	int GetAnimationNum() { return Player_Animation_Number; };
	float GetParallaxScale() { return ParallaxScale; };
	float GetParallaxBias() { return ParallaxBias; };
	int GetTerrainMappingMode() { return Terrain_Mapping_mode; };

	// Component Sets Func
	CComponentSet* GetComponentSet(int num)
	{
		dummy.first = num;

		std::set<CoptSetPair, Comp_ComponentSet>::iterator it = m_sComponentSets.find(dummy);
		if (it == m_sComponentSets.end())
			return nullptr;

		return it->second.get();
	}

	void SetComponentSet(std::shared_ptr<CComponentSet>& pComponentSet) {
		static int ComponentSets_Num = 0;
		std::shared_ptr<CComponentSet> pSet = pComponentSet;
		CoptSetPair pair = std::make_pair(ComponentSets_Num++, pSet);
		m_sComponentSets.insert(pair);
	}

	std::set<CoptSetPair, Comp_ComponentSet>& GetComponentSetRoot() { return m_sComponentSets; };
};