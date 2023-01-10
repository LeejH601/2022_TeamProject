#pragma once
#include "..\Global\stdafx.h"

class CTexture;
class CImGuiManager
{
private:
	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);;
	float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_my_window = true;

	std::unique_ptr<CTexture> m_pTexture;

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
	void OnPreRender();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	ImVec4 GetColor() {	return clear_color; };
};