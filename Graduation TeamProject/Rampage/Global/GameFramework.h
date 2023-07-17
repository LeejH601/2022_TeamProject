#pragma once
#include "stdafx.h"
#include "Locator.h"
#include "Timer.h"
#include "..\Scene\Scene.h"
#include "..\Scene\SceneManager.h"
#include "..\Shader\PostProcessShader.h"

class CCamera;
class CGameObject;
class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//DXGI 팩토리 인터페이스에 대한 포인터이다.
	ComPtr<IDXGIFactory4> m_pdxgiFactory;

	//스왑 체인 인터페이스에 대한 포인터이다.
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain;

	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ComPtr<ID3D12Device> m_pd3dDevice;

	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	//스왑 체인의 후면 버퍼의 개수와 현재 스왑 체인의 푸면 버퍼 인덱스이다.
	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pd3dSwapRTVCPUHandles[m_nSwapChainBuffers];

	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvDescriptorCPUHandle;

	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;

	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;

	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.
	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	//사용자 화면 해상도를 가져옵니다.
	RECT m_DeskTopCoordinatesRect;

	std::unique_ptr<CGameObject> m_pPlayer = NULL;
	std::unique_ptr<CSceneManager> m_pSceneManager = NULL;

	std::vector<std::unique_ptr<IMessageListener>> m_pListeners;

	CGameTimer					m_GameTimer;

	_TCHAR						m_pszFrameRate[50];

	DWORD dwDirection;
	bool m_bIsSprint = false;
public:
	CGameFramework();
	~CGameFramework();

	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	// 얘네는 뭐라 설명하지
	void InitSound();
	void InitLocator();
	void InitImgui();

	//렌더타겟을 렌더링하기 위한 준비를 하는 함수이다.
	void PrepareImGui();
	void PrepareRenderTarget();
	void OnPostRender();

	//렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다. 
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects();
	void ReleaseObjects();

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void UpdateObjects();
	void FrameAdvance();
	void MoveToNextFrame();
	void RenderObjects();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//전체화면 모드 <-> 윈도우 모드의 전환을 구현하는 함수이다.
	void ChangeSwapChainState(bool bFullScreenState);
	void ExecuteCommandLists();
};