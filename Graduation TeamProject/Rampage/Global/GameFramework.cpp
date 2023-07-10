#include "GameFramework.h"
#include "..\Global\Camera.h"
#include "..\Scene\SceneManager.h"
#include "..\Scene\MainScene.h"
#include "..\Scene\LobbyScene.h"
#include "..\Scene\SimulatorScene.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Object\Texture.h"
#include "..\Shader\DepthRenderShader.h"
#include "..\Shader\PostProcessShader.h"
#include <windowsx.h>
#include "..\Sound\SoundManager.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Scene\SceneManager.h"

CLocator Locator;

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

	InitSound();
	InitLocator();

	//Direct3D ����̽�, ��� ť�� ��� ����Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�. 
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	//�������� ���� ��ü�� �����Ѵ�.
	BuildObjects();

	//ImGui �������� ���� ������ �մϴ�.
	InitImgui();

	//CommandList�� �����ϰ� GPU ������ �Ϸ�� ������ ��ٸ��ϴ�.
	ExecuteCommandLists();

	return(true);
}
void CGameFramework::OnDestroy()
{
	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���. 
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	//���� ��ü(���� ���� ��ü)�� �Ҹ��Ѵ�. 
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);

	CImGuiManager::GetInst()->OnDestroy();
}
void CGameFramework::InitSound()
{
	//Init FMOD
	CSoundManager::GetInst()->Init();

	//Register Sound
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 1 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 2 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 3 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 4 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 5 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 1 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 2 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 3 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 4 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 5 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);

	CSoundManager::GetInst()->RegisterSound("Sound/David Bowie - Starman.mp3", false, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Air Cut by Langerium Id-84616.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Bloody Blade 2 by Kreastricon62 Id-323526.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Swing by XxChr0nosxX Id-268227.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Sword by hello_flowers Id-37596.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Sword4 by Streety Id-30246.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/Sword7 by Streety Id-30248.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/ethanchase7744__sword-slash.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	//CSoundManager::GetInst()->RegisterSound("Sound/shoot/sword-with-swipe.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/shoot/thebuilder15__sword-impact.wav", false, SOUND_CATEGORY::SOUND_SHOOT);

	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh1.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh2.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh3.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh4.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh5.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/EffectSound1.mp3", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/herkules92__sword-attack.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/herkules92__sword-attack.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/hit-swing-sword.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/kneeling__cleaver.mp3", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/MP_Left Hook.mp3", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/Buffer Spell.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/shield-guard-6963.mp3", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/Destruction_Sound_Effect_1.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/Destruction_Sound_Effect_2.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/440773__mgamabile__smashing-glass.wav", false, SOUND_CATEGORY::SOUND_SHOCK);


	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Goblin/GoblinMoan01.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Goblin/GoblinMoan02.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Goblin/GoblinMoan03.mp3", false, SOUND_CATEGORY::SOUND_VOICE);

	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan01.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan02.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan03.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan04.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan05.mp3", false, SOUND_CATEGORY::SOUND_VOICE);
	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Orc/OrcMoan06.mp3", false, SOUND_CATEGORY::SOUND_VOICE);

	CSoundManager::GetInst()->RegisterSound("Sound/Voice/Skeleton/SkeletonMoan01.mp3", false, SOUND_CATEGORY::SOUND_VOICE);

	CSoundManager::GetInst()->RegisterSound("Sound/UI/Menu Selection Click by NenadSimic Id-171697.wav", false, SOUND_CATEGORY::SOUND_UI_BUTTON);
	CSoundManager::GetInst()->RegisterSound("Sound/UI/Water Click by Mafon2 Id-371274.wav", false, SOUND_CATEGORY::SOUND_UI_BUTTON_CLICK);

	// ȣ���� ���� ������ ���
	std::unique_ptr<SoundPlayComponent> listener = std::make_unique<SoundPlayComponent>();
	listener->SetDelay(0.0f);
	listener->SetEnable(true);
	listener->SetSC(SOUND_CATEGORY::SOUND_UI_BUTTON);
	listener->SetMT(MONSTER_TYPE::NONE);
	listener->SetSoundNumber(0);
	listener->SetVolume(1.0f);
	m_pListeners.push_back(std::move(listener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), nullptr);

	// Ŭ�� ���� ������ ���
	listener = std::make_unique<SoundPlayComponent>();
	listener->SetDelay(0.0f);
	listener->SetEnable(true);
	listener->SetSC(SOUND_CATEGORY::SOUND_UI_BUTTON_CLICK);
	listener->SetMT(MONSTER_TYPE::NONE);
	listener->SetSoundNumber(0);
	listener->SetVolume(1.0f);
	m_pListeners.push_back(std::move(listener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), nullptr);


	//CSoundManager::GetInst()->PlaySound("Sound/Background/Action 2 (Loop).wav", 0.25f, 0.0f);
}
void CGameFramework::InitLocator()
{
	Locator.Init();
	Locator.SetTimer(&m_GameTimer);
}
void CGameFramework::InitImgui()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);

	CImGuiManager::GetInst()->Init(m_hWnd, m_pd3dDevice.Get(), m_pd3dCommandList.Get(), d3dRtvCPUDescriptorHandle, m_DeskTopCoordinatesRect);
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
		//����� ����ũž�� �ػ󵵸� �����ɴϴ�.
		ComPtr<IDXGIOutput> pd3dOutput = NULL;
		DXGI_OUTPUT_DESC pd3dOutputDesc;

		pd3dAdapter->EnumOutputs(0, pd3dOutput.GetAddressOf());
		pd3dOutput->GetDesc(&pd3dOutputDesc);

		m_DeskTopCoordinatesRect = pd3dOutputDesc.DesktopCoordinates;

		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)m_pd3dDevice.GetAddressOf())))
			break;
	}

	//��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�. 
	if (!m_pd3dDevice)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)pd3dAdapter.GetAddressOf());
		D3D12CreateDevice(pd3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)m_pd3dDevice.GetAddressOf());
	}

	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�. 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�. 
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�. 
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�. 
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)m_pd3dFence.GetAddressOf());
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;

	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�). �̺�Ʈ�� ����Ǹ�(Signal) ��
	��Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Locator.SetDevice(m_pd3dDevice.Get());
}
void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	//����(Direct) ��� ť�� �����Ѵ�. 
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)m_pd3dCommandQueue.GetAddressOf());

	//����(Direct) ��� �Ҵ��ڸ� �����Ѵ�.
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)m_pd3dCommandAllocator.GetAddressOf());

	//����(Direct) ��� ����Ʈ�� �����Ѵ�. 
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)m_pd3dCommandList.GetAddressOf());

	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����. 
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

	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�. 
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dRtvDescriptorHeap.GetAddressOf());

	//����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dDsvDescriptorHeap.GetAddressOf());
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
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����Ѵ�. 
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
	//����-���ٽ� ���۸� �����Ѵ�. 
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


	//����-���ٽ� ���� �並 �����Ѵ�. 
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

		DXGI_FORMAT pdxgiResourceFormats[6] = { DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };
		m_pSceneManager->GetMainScene()->m_pPostProcessShader->CreateResourcesAndViews(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 6, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, 6 + 1); //SRV to (Render Targets) + (Depth Buffer)
		d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * 6);
		CSimulatorScene::GetInst()->m_pPostProcessShader->CreateResourcesAndViews(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 6, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, 6 + 1); //SRV to (Render Targets) + (Depth Buffer)
		CImGuiManager::GetInst()->GetRtvDescHandle();
	}

	((CMainTMPScene*)m_pSceneManager->GetMainScene())->m_pBloomComputeShader->SetTextureSource(m_pd3dDevice.Get(), m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetTextureShared());

	((CMainTMPScene*)m_pSceneManager->GetMainScene())->m_pHDRComputeShader->SetTextureSource(m_pd3dDevice.Get(), m_pSceneManager->GetMainScene()->m_pPostProcessShader->GetTextureShared());

	CSimulatorScene::GetInst()->m_pBloomComputeShader->SetTextureSource(m_pd3dDevice.Get(), CSimulatorScene::GetInst()->m_pPostProcessShader->GetTextureShared());

	CSimulatorScene::GetInst()->m_pHDRComputeShader->SetTextureSource(m_pd3dDevice.Get(), CSimulatorScene::GetInst()->m_pPostProcessShader->GetTextureShared());
	
	m_pPlayer = std::make_unique<CKnightPlayer>(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), 1); 
	m_pPlayer->SetPosition(XMFLOAT3(57.0f, 3.5f, 225.0f));
	m_pPlayer->SetScale(2.0f, 2.0f, 2.0f);
	m_pPlayer->Rotate(0.0f, 165.0f, 0.0f);
	((CPlayer*)m_pPlayer.get())->m_pStateMachine->ChangeState(Idle_Player::GetInst());

	m_pSceneManager->SetPlayer((CPlayer*)m_pPlayer.get());
}
void CGameFramework::ReleaseObjects()
{
	// �޸� ������ �ʿ��� ��ü���� �޸𸮸� �������ش�.

	// Release Fmod Llibrary
	CSoundManager::GetInst()->Release();

	Locator.GetPxScene()->release();
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
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
	switch (nMessageID)
	{
	case WM_ACTIVATE:

		break;
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
	
	m_pSceneManager->ProcessInput(m_hWnd, dwDirection, m_GameTimer.GetFrameTimeElapsed());
}
void CGameFramework::UpdateObjects()
{
	m_pSceneManager->Update(m_GameTimer.GetFrameTimeElapsed());
	CSoundManager::GetInst()->UpdateSound();
}
void CGameFramework::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CGameFramework::PrepareRenderTarget()
{
	//ImVec4 clear_color = CImGuiManager::GetInst()->GetColor();
	const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };

	m_pd3dCommandList->ClearRenderTargetView(m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], clear_color_with_alpha, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], FALSE, &m_d3dDsvDescriptorCPUHandle);
}
void CGameFramework::PrepareImGui()
{
	HRESULT hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	CImGuiManager::GetInst()->OnPrepareRender(m_pd3dCommandList.Get(), &m_d3dDsvDescriptorCPUHandle, m_GameTimer.GetFrameTimeElapsed(), m_GameTimer.GetTotalTime(), NULL);

	//��� ����Ʈ�� ���� ���·� �����. 
	hResult = m_pd3dCommandList->Close();

	//��� ����Ʈ�� ��� ť�� �߰��Ͽ� �����Ѵ�.
	ID3D12CommandList* CommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, CommandLists);

	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���.
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);
}
void CGameFramework::OnPostRender()
{
	m_pSceneManager->OnPostRender();
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
void CGameFramework::RenderObjects()
{
	//��� �Ҵ��ڸ� �����Ѵ�.
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	CScene* pScene = m_pSceneManager->GetCurrentScene();

	if (dynamic_cast<CLobbyScene*>(m_pSceneManager->GetCurrentScene()) && dynamic_cast<CLobbyScene*>(pScene)->GetSceneType() == (UINT)(LobbySceneType::SIMULATOR_Scene))
	{
		PrepareImGui();
	}


	//��� ����Ʈ�� �����Ѵ�.
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList.Get(), m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pSceneManager->PreRender(m_pd3dCommandList.Get(), m_GameTimer.GetFrameTimeElapsed());

	if (dynamic_cast<CMainTMPScene*>(pScene) || ((dynamic_cast<CLobbyScene*>(pScene)) && dynamic_cast<CLobbyScene*>(pScene)->GetSceneType() == (UINT)(LobbySceneType::LOGO_Scene)))
	{
		m_pSceneManager->GetCurrentScene()->SetHDRRenderSource(m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get());
		m_pSceneManager->OnPrepareRenderTarget(m_pd3dCommandList.Get(), 1, &m_pd3dSwapRTVCPUHandles[m_nSwapChainBufferIndex], m_d3dDsvDescriptorCPUHandle);
	}
	else
		PrepareRenderTarget();

	m_pd3dCommandList->ClearDepthStencilView(m_d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	UpdateShaderVariables(m_pd3dCommandList.Get());

	m_pSceneManager->Render(m_pd3dCommandList.Get(), m_GameTimer.GetFrameTimeElapsed(), m_GameTimer.GetTotalTime(), NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList.Get(), m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//��� ����Ʈ�� ���� ���·� �����. 
	hResult = m_pd3dCommandList->Close();

	//��� ����Ʈ�� ��� ť�� �߰��Ͽ� �����Ѵ�.
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���.
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	OnPostRender();
}
void CGameFramework::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);
	
	ProcessInput();
	UpdateObjects();

	RenderObjects();
	
	/*����ü���� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� ��
	������ �ٲ� ���̴�.*/
	m_pdxgiSwapChain->Present(0, 0);

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 34, 15);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}
void CGameFramework::ChangeSwapChainState()
{
	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	// ���� ��üȭ�� �������� Ȯ���ϰ� ! �������� ���� ������ �ݴ�� ��ȯ�Ѵ�.
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
	//�� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰��Ѵ�. 
	m_pd3dCommandList->Close();
	ComPtr<ID3D12CommandList> ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists->GetAddressOf());

	::WaitForGpuComplete(m_pd3dCommandQueue.Get(), m_pd3dFence.Get(), ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);
	m_GameTimer.Reset();
}
