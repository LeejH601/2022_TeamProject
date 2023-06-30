#pragma once
#include "../Global/stdafx.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Shader\PostProcessShader.h"
#include "..\Shader\HDRComputeShader.h"
#include "..\Shader\BloomShader.h"

class CCamera;
class CGameObject;
class CScene 
{
protected:
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = nullptr;
	ComPtr<ID3D12RootSignature> m_pd3dComputeRootSignature = nullptr;

	CGameObject* m_pPlayer = nullptr;
	std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
public:
	std::shared_ptr<CPostProcessShader> m_pPostProcessShader = NULL;
	std::shared_ptr<CHDRComputeShader> m_pHDRComputeShader;
	std::shared_ptr<CBloomShader> m_pBloomComputeShader;

public:
	CScene() {}
	virtual ~CScene() {}
	
	ID3D12RootSignature* GetGraphicsRootSignature() { 
		return(m_pd3dGraphicsRootSignature.Get()); 
	}
	virtual void SetPlayer(CGameObject* pPlayer) { m_pPlayer = pPlayer; }

	void SetHDRRenderSource(ID3D12Resource* RenderTargetResource) { 
		if(m_pHDRComputeShader)
			m_pHDRComputeShader->m_pRenderTargetResource = RenderTargetResource; 
	};

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) {}
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual void CreateComputeRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetComputeRootSignature() { return(m_pd3dComputeRootSignature.Get()); }

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual SCENE_RETURN_TYPE OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection) { return SCENE_RETURN_TYPE::NONE; }
	
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseObjects() {}

	virtual bool ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed) { return false; }
	virtual void Enter(HWND hWnd) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle){}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera = NULL) {}
	virtual void OnPostRender() {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) {}
	virtual void HandleCollision(const CollideParams& params) {}
	virtual void HandleOnGround(const OnGroundParams& params) {}
};