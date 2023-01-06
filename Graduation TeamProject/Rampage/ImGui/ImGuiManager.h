#pragma once
#include "..\Global\stdafx.h"

class CImGuiManager
{
private:
	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);;
	float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_my_window = true;

	ComPtr<ID3D12DescriptorHeap> m_pd3dSrvDescHeap = NULL;
public:
	DECLARE_SINGLE(CImGuiManager);
	
	CImGuiManager();
	~CImGuiManager();
	void CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice);
	void Init(HWND hWnd, ID3D12Device* pd3dDevice);

	void OnPreRender();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	ImVec4 GetColor() {	return clear_color; };
};