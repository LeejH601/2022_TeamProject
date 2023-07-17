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

	//DXGI ���丮 �������̽��� ���� �������̴�.
	ComPtr<IDXGIFactory4> m_pdxgiFactory;

	//���� ü�� �������̽��� ���� �������̴�.
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain;

	//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ComPtr<ID3D12Device> m_pd3dDevice;

	//MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	//���� ü���� �ĸ� ������ ������ ���� ���� ü���� Ǫ�� ���� �ε����̴�.
	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pd3dSwapRTVCPUHandles[m_nSwapChainBuffers];

	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvDescriptorCPUHandle;

	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;

	//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;

	//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.
	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

	//����� ȭ�� �ػ󵵸� �����ɴϴ�.
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

	//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	// ��״� ���� ��������
	void InitSound();
	void InitLocator();
	void InitImgui();

	//����Ÿ���� �������ϱ� ���� �غ� �ϴ� �Լ��̴�.
	void PrepareImGui();
	void PrepareRenderTarget();
	void OnPostRender();

	//���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�. 
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects();
	void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void UpdateObjects();
	void FrameAdvance();
	void MoveToNextFrame();
	void RenderObjects();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//��üȭ�� ��� <-> ������ ����� ��ȯ�� �����ϴ� �Լ��̴�.
	void ChangeSwapChainState(bool bFullScreenState);
	void ExecuteCommandLists();
};