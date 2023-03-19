#include "GameFramework.h"
#include "..\Global\Camera.h"
#include "..\Scene\SceneManager.h"
#include "..\Scene\MainScene.h"
#include "..\Scene\SimulatorScene.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Object\Texture.h"
#include "..\Shader\DepthRenderShader.h"
#include "Locator.h"
#include "MessageDispatcher.h"
#include "..\Shader\PostProcessShader.h"
#include <windowsx.h>

CGameFramework::CGameFramework()
{
	m_nSwapChainBufferIndex = 0;
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("Graduation TeamProject - Rampage ("));
}
CGameFramework::~CGameFramework()
{
}
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	//렌더링할 게임 객체를 생성한다.
	BuildObjects();

	//ImGui 렌더링을 위한 세팅을 합니다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);
	CImGuiManager::GetInst()->Init(m_hWnd, m_pd3dDevice.Get(), m_pd3dCommandList.Get(), d3dRtvCPUDescriptorHandle);

	//CommandList를 실행하고 GPU 연산이 완료될 때까지 기다립니다.
	ExecuteCommandLists();




	/*physx::PxPvd* mPvd = physx::PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);

	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);*/

	return(true);
}
void CGameFramework::OnDestroy()
{
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다. 
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	//게임 객체(게임 월드 객체)를 소멸한다. 
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
}
void CGameFramework::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue.Get(), &dxgiSwapChainDesc, (IDXGISwapChain**)m_pdxgiSwapChain.GetAddressOf());

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}
void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;
	UINT nDXGIFactoryFlags = 0;

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)pd3dDebugController.GetAddressOf());
	if (pd3dDebugController)
		pd3dDebugController->EnableDebugLayer();
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)m_pdxgiFactory.GetAddressOf());
	ComPtr<IDXGIAdapter1> pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, pd3dAdapter.GetAddressOf()); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)m_pd3dDevice.GetAddressOf())))
			break;
	}

	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다. 
	if (!m_pd3dDevice)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)pd3dAdapter.GetAddressOf());
		D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)m_pd3dDevice.GetAddressOf());
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다. 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다. 
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다. 
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	//펜스를 생성하고 펜스 값을 0으로 설정한다. 
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)m_pd3dFence.GetAddressOf());
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;

	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다). 이벤트가 실행되면(Signal) 이
	벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}
void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	//직접(Direct) 명령 큐를 생성한다. 
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)m_pd3dCommandQueue.GetAddressOf());

	//직접(Direct) 명령 할당자를 생성한다.
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)m_pd3dCommandAllocator.GetAddressOf());

	//직접(Direct) 명령 리스트를 생성한다. 
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)m_pd3dCommandList.GetAddressOf());

	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다. 
	hResult = m_pd3dCommandList->Close();
}
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + 6 + 1 + 6;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다. 
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dRtvDescriptorHeap.GetAddressOf());

	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dDsvDescriptorHeap.GetAddressOf());
}
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	//스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)m_ppd3dRenderTargetBuffers[i].GetAddressOf());
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i].Get(), &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		m_pd3dSwapRTVCPUHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}
}
void CGameFramework::CreateDepthStencilView()
{
	//깊이-스텐실 버퍼를 생성한다. 
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)m_pd3dDepthStencilBuffer.GetAddressOf());


	//깊이-스텐실 버퍼 뷰를 생성한다. 
	m_d3dDsvDescriptorCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer.Get(), NULL, m_d3dDsvDescriptorCPUHandle);
}
void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	m_pSceneManager = std::make_unique<CSceneManager>(m_pd3dDevice.Get(), m_pd3dCommandList.Get());

	{
		D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * (m_nSwapChainBuffers + 1));

		DXGI_FORMAT pdxgiResourceFormats[6] = { DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT };
		m_pSceneManager->GetMainScene()->m_pPostProcessShader->CreateResourcesAndViews(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 6, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, 6 + 1); //SRV to (Render Targets) + (Depth Buffer)
		d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * 6);
		CSimulatorScene::GetInst()->m_pPostProcessShader->CreateResourcesAndViews(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 6, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, 6 + 1); //SRV to (Render Targets) + (Depth Buffer)
		/*CSimulatorScene::GetInst()->m_pPostProcessShader->SetTexture(m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetTextureShared());
		CSimulatorScene::GetInst()->m_pPostProcessShader->SetRtvCPUDescriptorHandle(m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetRtvCPUDescriptorHandle(), 6);*/

		/*D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUDescriptorHandles = CSimulatorScene::GetInst()->m_pPostProcessShader->GetRtvCPUDescriptorHandle();
		pd3dRtvCPUDescriptorHandles = m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetRtvCPUDescriptorHandle();*/

		/*D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
		d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
		d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUDescriptorHandles = CSimulatorScene::GetInst()->m_pPostProcessShader->GetRtvCPUDescriptorHandle();
		pd3dRtvCPUDescriptorHandles = m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetRtvCPUDescriptorHandle();*/
		
		/*int nResources = CSimulatorScene::GetInst()->m_pPostProcessShader->GetTexture()->GetTextures();
		pd3dRtvCPUDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nResources];

		for (UINT i = 0; i < nResources; i++)
		{
			d3dRenderTargetViewDesc.Format = pdxgiResourceFormats[i];
			ID3D12Resource* pd3dTextureResource = CSimulatorScene::GetInst()->m_pPostProcessShader->GetTexture()->GetResource(i);
			m_pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
			pd3dRtvCPUDescriptorHandles[i] = d3dRtvCPUDescriptorHandle;
			d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
		}*/

		CImGuiManager::GetInst()->GetRtvDescHandle();
		

		//DXGI_FORMAT pdxgiDepthSrvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
		//m_pPostProcessShader->CreateShaderResourceViews(m_pd3dDevice.Get(), 1, &m_pd3dDepthStencilBuffer, pdxgiDepthSrvFormats);
	}

	((CMainTMPScene*)m_pSceneManager->GetMainScene())->m_pHDRComputeShader->SetTextureSource(m_pd3dDevice.Get(), m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetTextureShared());
	//((CMainTMPScene*)m_pSceneManager->GetMainScene())->m_pHDRComputeShader->SetTextureSource(m_pd3dDevice.Get(), m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetTextureShared());

	m_pFloatingCamera = std::make_unique<CFloatingCamera>();
	m_pFloatingCamera->Init(m_pd3dDevice.Get(), m_pd3dCommandList.Get());
	m_pFloatingCamera->SetPosition(XMFLOAT3(0.0f, 400.0f, -100.0f));

	Locator.CreateMainSceneCamera(m_pd3dDevice.Get(), m_pd3dCommandList.Get());

	m_pCurrentCamera = m_pFloatingCamera.get();

	m_pPlayer = std::make_unique<CPlayer>(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 1);

	m_pSceneManager->SetPlayer((CPlayer*)m_pPlayer.get());
	((CThirdPersonCamera*)Locator.GetMainSceneCamera())->SetPlayer((CPlayer*)m_pPlayer.get());
}
void CGameFramework::ReleaseObjects()
{
	// 메모리 해제가 필요한 객체들의 메모리를 해제해준다.
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		//if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE) {
		//	//::SetCapture(hWnd);
		//	m_ptOldCursorPos.x = GET_X_LPARAM(lParam);
		//	m_ptOldCursorPos.y = GET_Y_LPARAM(lParam);
		//	::GetCursorPos(&m_ptOldCursorPos);
		//}
		break;
	default:
		break;
	}

	m_pSceneManager->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case '1':
			m_pCurrentCamera = m_pFloatingCamera.get();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
			break;
		case '2':
			m_pCurrentCamera = Locator.GetMainSceneCamera();
			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::THIRD_FERSON_MODE);
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);
			break;
		case VK_F9:
			ChangeSwapChainState();
		default:
			break;
		}
		break;
	default:
		break;
	}

	m_pSceneManager->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, dwDirection);
}
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static int CursorHideCount = 0;
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		/*if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();*/
		RECT screenRect;
		GetWindowRect(hWnd, &screenRect);
		MOUSE_CUROSR_MODE eMouseMode = Locator.GetMouseCursorMode();
		switch (eMouseMode)
		{
		case MOUSE_CUROSR_MODE::THIRD_FERSON_MODE:
			if (CursorHideCount < 1) {
				CursorHideCount++;
				ShowCursor(false);
				ClipCursor(&screenRect);
			}
			break;
		case MOUSE_CUROSR_MODE::FLOATING_MODE:
			while (CursorHideCount > 0)
			{
				CursorHideCount--;
				ShowCursor(true);
			}
			ClipCursor(NULL);
			break;
		default:
			break;
		}
		break;
	}
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}
void CGameFramework::ProcessInput()
{
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	/*if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE) {
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		m_ptOldCursorPos.x = ptCursorPos.x;
		m_ptOldCursorPos.y = ptCursorPos.y;
	}*/

	m_pCurrentCamera->ProcessInput(dwDirection, cxDelta, cyDelta, m_GameTimer.GetFrameTimeElapsed());
	((CPlayer*)(m_pPlayer.get()))->ProcessInput(dwDirection, cxDelta, cyDelta, m_GameTimer.GetFrameTimeElapsed(), m_pCurrentCamera);
}
void CGameFramework::AnimateObjects()
{
	// Object들의 애니메이션을 수행한다.
	m_pFloatingCamera->Animate(m_GameTimer.GetFrameTimeElapsed());
	Locator.GetMainSceneCamera()->Animate(m_GameTimer.GetFrameTimeElapsed());
	m_pSceneManager->Animate(m_GameTimer.GetFrameTimeElapsed());
}
void CGameFramework::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Parallax_Info) + 255) & ~255); //256의 배수
	m_pd3dcbParallax = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbParallax->Map(0, NULL, (void**)&m_pcbMappedParallax);
}
void CGameFramework::OnPrepareRenderTarget()
{
	//ImVec4 clear_color = CImGuiManager::GetInst()->GetColor();
	const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };

	/*FLOAT pfDefaultClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	D3D12_CPU_DESCRIPTOR_HANDLE* pd3dAllRtvCPUHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[2];

	pd3dAllRtvCPUHandles[0] = m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex];
	m_pd3dCommandList->ClearRenderTargetView(m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], clear_color_with_alpha, 0, NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList.Get(), CImGuiManager::GetInst()->GetRTTextureResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = CImGuiManager::GetInst()->GetRtvCPUDescriptorHandle();
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfDefaultClearColor, 0, NULL);
	pd3dAllRtvCPUHandles[1] = d3dRtvCPUDescriptorHandle;

	m_pd3dCommandList->OMSetRenderTargets(2, pd3dAllRtvCPUHandles, FALSE, &m_d3dDsvDescriptorCPUHandle);


	if (pd3dAllRtvCPUHandles) delete[] pd3dAllRtvCPUHandles;*/
	m_pd3dCommandList->ClearRenderTargetView(m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], clear_color_with_alpha, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], FALSE, &m_d3dDsvDescriptorCPUHandle);
}
void CGameFramework::OnPrepareImGui()
{
	HRESULT hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	CImGuiManager::GetInst()->OnPrepareRender(m_pd3dCommandList.Get(), &m_d3dDsvDescriptorCPUHandle, m_GameTimer.GetFrameTimeElapsed(), m_GameTimer.GetTotalTime(), m_pCurrentCamera);

	//명령 리스트를 닫힌 상태로 만든다. 
	hResult = m_pd3dCommandList->Close();

	//명령 리스트를 명령 큐에 추가하여 실행한다.
	ID3D12CommandList* CommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, CommandLists);

	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);
}
void CGameFramework::OnPostRenderTarget()
{
	m_pSceneManager->OnPostRenderTarget();
}
void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence.Get(), nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}
void CGameFramework::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*m_pcbMappedParallax->m_fParallaxScale = CImGuiManager::GetInst()->GetParallaxScale();
	m_pcbMappedParallax->m_fParallaxBias = CImGuiManager::GetInst()->GetParallaxBias();
	m_pcbMappedParallax->m_iMappingMode = CImGuiManager::GetInst()->GetTerrainMappingMode() % 3;
	if (m_pcbMappedParallax->m_iMappingMode < 0) m_pcbMappedParallax->m_iMappingMode = 0;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbParallax->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);*/
}
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);
	//if (started == 0) {
	//	Locator.GetPxScene()->simulate(m_GameTimer.GetFrameTimeElapsed());
	//	started++;
	//}


	ProcessInput();
	AnimateObjects();

	XMFLOAT3 xmf3PlayerPos = m_pPlayer->GetPosition();
	xmf3PlayerPos.y += 12.5f;

	m_pCurrentCamera->Update(xmf3PlayerPos, 0.0f);

	Locator.GetMessageDispather()->DispatchMessages();

	//명령 할당자를 리셋한다.
	HRESULT hResult = m_pd3dCommandAllocator->Reset();

	if (typeid(*m_pSceneManager->GetCurrentScene()) != typeid(CMainTMPScene))
		OnPrepareImGui();

	//명령 리스트를 리셋한다.
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList.Get(), m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pSceneManager->PreRender(m_pd3dCommandList.Get(), m_GameTimer.GetFrameTimeElapsed());

	if (typeid(*m_pSceneManager->GetCurrentScene()) == typeid(CMainTMPScene)) {
		m_pSceneManager->GetCurrentScene()->SetHDRRenderSource(m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get());
		m_pSceneManager->OnPrepareRenderTarget(m_pd3dCommandList.Get(), 1, &m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], m_d3dDsvDescriptorCPUHandle);
	}
	else
		OnPrepareRenderTarget();

	m_pd3dCommandList->ClearDepthStencilView(m_d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	UpdateShaderVariables(m_pd3dCommandList.Get());

	m_pSceneManager->Render(m_pd3dCommandList.Get(), m_GameTimer.GetFrameTimeElapsed(), m_GameTimer.GetTotalTime(), m_pCurrentCamera);


	::SynchronizeResourceTransition(m_pd3dCommandList.Get(), m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//명령 리스트를 닫힌 상태로 만든다. 
	hResult = m_pd3dCommandList->Close();

	//명령 리스트를 명령 큐에 추가하여 실행한다.
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	OnPostRenderTarget();

	/*스왑체인을 프리젠트한다. 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인
	덱스가 바뀔 것이다.*/
	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 34, 15);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}
void CGameFramework::ChangeSwapChainState()
{
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	// 현재 전체화면 상태인지 확인하고 ! 연산으로 현재 상태의 반대로 전환한다.
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dRenderTargetBuffers[i]) m_ppd3dRenderTargetBuffers[i].Reset();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	CreateRenderTargetViews();
}
void CGameFramework::ExecuteCommandLists()
{
	//씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다. 
	m_pd3dCommandList->Close();
	ComPtr<ID3D12CommandList> ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists->GetAddressOf());

	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);
	m_GameTimer.Reset();
}
