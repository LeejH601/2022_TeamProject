#pragma once
#include "../Global/stdafx.h"

class CCamera;
class CScene 
{
protected:
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = nullptr;
public:
	CScene() {}
	virtual ~CScene() {}
	
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature.Get()); }

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) {}
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void CheckCollide() {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) {}
};