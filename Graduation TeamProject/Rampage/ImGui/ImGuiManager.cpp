#include "ImGuiManager.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Scene\SimulatorScene.h"
#include "..\Object\Texture.h"
#include "..\Global\GameFramework.h"
#include "..\Global\Locator.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"
#include "..\Object\BillBoardComponent.h"
#include "..\Object\TextureManager.h"
#include "..\Object\ParticleComponent.h"
#include "..\Object\AnimationComponent.h"

#define NUM_FRAMES_IN_FLIGHT 3
//========================================================================
void DataLoader::SaveComponentSets()
{
	std::string path;
	FILE* pInFile;

	for (int i = 0; i < 3; ++i)
	{
		CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();

		// State 선택 로직 생각 필요
		switch (i) {
		case 0:
			pCurrentAnimation = Atk1_Player::GetInst();
			break;
		case 1:
			pCurrentAnimation = Atk2_Player::GetInst();
			break;
		case 2:
			pCurrentAnimation = Atk3_Player::GetInst();
			break;
		default:
			break;
		}

		path = file_path + std::to_string(i) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "wb");

		SaveComponentSet(pInFile, pCurrentAnimation);

		fclose(pInFile);
	}
}

void DataLoader::LoadComponentSets()
{
	std::string path;
	FILE* pInFile;

	for (int i = 0; i < 3; ++i)
	{
		CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();

		// State 선택 로직 생각 필요
		switch (i) {
		case 0:
			pCurrentAnimation = Atk1_Player::GetInst();
			break;
		case 1:
			pCurrentAnimation = Atk2_Player::GetInst();
			break;
		case 2:
			pCurrentAnimation = Atk3_Player::GetInst();
			break;
		default:
			break;
		}
		path = file_path + std::to_string(0) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "rb");
		if (!pInFile)
			continue;
		LoadComponentSet(pInFile, pCurrentAnimation);
		fclose(pInFile);
	}
}

void DataLoader::SaveComponentSet(FILE* pInFile, CState<CPlayer>* pState)
{
	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pState->GetCameraMoveComponent());
	CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pState->GetCameraShakeComponent());
	CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pState->GetCameraZoomerComponent());

	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pState->GetDamageAnimationComponent());
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pState->GetShakeAnimationComponent());
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pState->GetStunAnimationComponent());

	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShockSoundComponent());
	SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShootSoundComponent());

	std::string str = "<Components>:";
	WriteStringFromFile(pInFile, str);

	WriteStringFromFile(pInFile, std::string("<CCameraMover>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraMoveComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetMovingTime());
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, pCameraMoveComponent->GetRollBackTime());
	WriteStringFromFile(pInFile, std::string("</CCameraMover>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraShaker>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraShakerComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Duration>:"));
	WriteFloatFromFile(pInFile, pCameraShakerComponent->GetDuration());
	WriteStringFromFile(pInFile, std::string("<Magnitude>:"));
	WriteFloatFromFile(pInFile, pCameraShakerComponent->GetMagnitude());
	WriteStringFromFile(pInFile, std::string("</CCameraShaker>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraZoomer>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pCameraZoomerComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetMovingTime());
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, pCameraZoomerComponent->GetRollBackTime());
	WriteStringFromFile(pInFile, std::string("<IsZoomIN>:"));
	WriteIntegerFromFile(pInFile, pCameraZoomerComponent->GetIsIn());
	WriteStringFromFile(pInFile, std::string("</CCameraZoomer>:"));

	WriteStringFromFile(pInFile, std::string("<DamageAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pDamageAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetMaxDistance());
	WriteStringFromFile(pInFile, std::string("<Speed>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetSpeed());
	WriteStringFromFile(pInFile, std::string("</DamageAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<ShakeAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShakeAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Distance>:"));
	WriteFloatFromFile(pInFile, pShakeAnimationComponent->GetDistance());
	WriteStringFromFile(pInFile, std::string("<Frequency>:"));
	WriteFloatFromFile(pInFile, pShakeAnimationComponent->GetFrequency());
	WriteStringFromFile(pInFile, std::string("</ShakeAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<StunAnimationComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pStunAnimationComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<StunTime>:"));
	WriteFloatFromFile(pInFile, pStunAnimationComponent->GetStunTime());
	WriteStringFromFile(pInFile, std::string("</StunAnimationComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CEffectSoundComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShockSoundComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pShockSoundComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pShockSoundComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pShockSoundComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</CEffectSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CShootSoundComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pShootSoundComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pShootSoundComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pShootSoundComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pShootSoundComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</CShootSoundComponent>:"));

	// 데미지 사운드 없는거 아닌가?
	/*WriteStringFromFile(pInFile, std::string("<CDamageSoundComponent>:"));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, pDamageSoundComponent->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, pDamageAnimationComponent->GetSoundNumber());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetDelay());
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, pDamageAnimationComponent->GetVolume());
	WriteStringFromFile(pInFile, std::string("</CDamageSoundComponent>:"));*/

	str = "</Components>:";
	WriteStringFromFile(pInFile, str);
}

void DataLoader::LoadComponentSet(FILE* pInFile, CState<CPlayer>* pState)
{
	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pState->GetCameraMoveComponent());
	CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pState->GetCameraShakeComponent());
	CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pState->GetCameraZoomerComponent());

	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pState->GetDamageAnimationComponent());
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pState->GetShakeAnimationComponent());
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pState->GetStunAnimationComponent());

	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShockSoundComponent());
	SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pState->GetShootSoundComponent());

	char buf[256];
	std::string str;
	str.resize(256);
	ReadStringFromFile(pInFile, buf);

	for (; ; )
	{
		ReadStringFromFile(pInFile, buf);

		if (!strcmp(buf, "<CCameraMover>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					pCameraMoveComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					pCameraMoveComponent->SetMovingTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					pCameraMoveComponent->SetRollBackTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraMoveComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraMover>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraShaker>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Duration>:"))
				{
					pCameraShakerComponent->SetDuration(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Magnitude>:"))
				{
					pCameraShakerComponent->SetMagnitude(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraShakerComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraShaker>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraZoomer>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					pCameraZoomerComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					pCameraZoomerComponent->SetMovingTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					pCameraZoomerComponent->SetRollBackTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<IsZoomIN>:"))
				{
					pCameraZoomerComponent->SetIsIn(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					pCameraZoomerComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraZoomer>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<DamageAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pDamageAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<MaxDistance>:"))
				{
					pDamageAnimationComponent->SetMaxDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Speed>:"))
				{
					pDamageAnimationComponent->SetSpeed(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</DamageAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<ShakeAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShakeAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Distance>:"))
				{
					pShakeAnimationComponent->SetDistance(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Frequency>:"))
				{
					pShakeAnimationComponent->SetFrequency(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</ShakeAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<StunAnimationComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pStunAnimationComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<StunTime>:"))
				{
					pStunAnimationComponent->SetStunTime(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</StunAnimationComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CEffectSoundComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShockSoundComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pShockSoundComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pShockSoundComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pShockSoundComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CEffectSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CShootSoundComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					pShootSoundComponent->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					pShootSoundComponent->SetSoundNumber(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					pShootSoundComponent->SetDelay(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					pShootSoundComponent->SetVolume(ReadFloatFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CShootSoundComponent>:"))
				{
					break;
				}
			}
		}
		// 없으니까 일단 주석처리
		/*else if (!strcmp(buf, "<CDamageSoundComponent>:"))
		{
			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					component->m_nSoundNumber = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					component->m_fDelay = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					component->m_fVolume = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "</CDamageSoundComponent>:"))
				{
					break;
				}
			}
		}*/
		else if (!strcmp(buf, "</Components>:"))
		{
			break;
		}
	}
}
//========================================================================

wchar_t* ConverCtoWC(const char* str)

{

	//wchar_t형 변수 선언

	wchar_t* pStr;

	//멀티 바이트 크기 계산 길이 반환

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	//wchar_t 메모리 할당

	pStr = new WCHAR[strSize];

	//형 변환

	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;

}



CImGuiManager::CImGuiManager()
{
}
CImGuiManager::~CImGuiManager()
{
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void CImGuiManager::CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = 3;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dSrvDescHeap.GetAddressOf());

	m_d3dSrvCPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dSrvGPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();

	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
}
void CImGuiManager::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}
ID3D12Resource* CImGuiManager::GetRTTextureResource() { return m_pRTTexture->GetResource(0); }
void CImGuiManager::Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle)
{
	CreateSrvDescriptorHeaps(pd3dDevice);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescHeap.Get(),
		m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	/*Locator.CreateSimulatorCamera(pd3dDevice, pd3dCommandList);
	m_pCamera = Locator.GetSimulaterCamera();*/

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };
	m_pRTTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	m_pRTTexture->CreateTexture(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue, RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, m_pRTTexture.get(), 1);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	ID3D12Resource* pd3dTextureResource = m_pRTTexture->GetResource(0);
	pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
	m_pd3dRtvCPUDescriptorHandles = d3dRtvCPUDescriptorHandle;

	m_pDataLoader = std::make_unique<DataLoader>();
	m_pDataLoader->LoadComponentSets();
}
void CImGuiManager::DemoRendering()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	//2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}
void CImGuiManager::SetUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CState<CPlayer>* pCurrentAnimation = Atk1_Player::GetInst();
	
	// State 선택 로직 생각 필요
	switch (Player_Animation_Number) {
	case 0:
		pCurrentAnimation = Atk1_Player::GetInst();
		break;
	case 1:
		pCurrentAnimation = Atk2_Player::GetInst();
		break;
	case 2:
		pCurrentAnimation = Atk3_Player::GetInst();
		break;
	default:
		break;
	}

	// Show my window.
	{
		ImGuiWindowFlags my_window_flags = 0;
		bool* p_open = NULL;

		my_window_flags |= ImGuiWindowFlags_NoResize;

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 25, main_viewport->WorkPos.y + 25));
		ImGui::SetNextWindowSize(ImVec2(FRAME_BUFFER_WIDTH - 50, FRAME_BUFFER_HEIGHT - 105), ImGuiCond_None);
		ImGui::Begin("Simulator", p_open, my_window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImVec2 initial_curpos = ImGui::GetCursorPos();
		{
			int my_image_width = 1050;
			int my_image_height = 600;
			ImGui::Image((ImTextureID)m_pRTTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr, ImVec2((float)my_image_width, (float)my_image_height));
		}
		ImVec2 button_pos = ImGui::GetCursorPos();

		// Menu Bar
		initial_curpos.x += 1055.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Impact Effect"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Particle Effect"))
		{
			//일단 보류
			//ParticleComponent* pParticleComponent = static_cast<ParticleComponent*>(pCurrentAnimation->GetParticleComponent());

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::Checkbox("On/Off##ParticleEffect", &pParticleComponent->GetEnable());

			//std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetParticleTextureList();
			//std::vector<const char*> items;
			//std::vector <std::string> str(100);
			//for (int i = 0; i < vTexture.size(); i++)
			//{
			//	std::wstring wstr = vTexture[i]->GetTextureName(0);
			//	str[i].assign(wstr.begin(), wstr.end());
			//	items.emplace_back(str[i].c_str());
			//}

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);

			//int iParticleSpriteN = pParticleComponent->GetParticleIndex();
			//ImGui::Combo("Texture##ParticleEffect", (int*)(&pParticleComponent->GetParticleIndex()), items.data(), items.size());
			//if (iParticleSpriteN != pParticleComponent->GetParticleIndex())
			//	pParticleComponent->SetTexture(0, ConverCtoWC(items[pParticleComponent->GetParticleIndex()]));

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragFloat("Size##ParticleEffect", &pParticleComponent->GetSize(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragFloat("Alpha##ParticleEffect", &pParticleComponent->GetAlpha(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragFloat("LifeTime##ParticleEffect", &pParticleComponent->GetLifeTime(), 0.01f, 0.0f, 10.0f, "%.1f", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragInt("ParticleCount##ParticleEffect", &pParticleComponent->GetParticleNumber(), 0.01f, 0.0f, 10.0f, "%d", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragFloat("Speed##ParticleEffect", &pParticleComponent->GetSpeed(), 0.01f, 0.0f, 10.0f, "%.1f", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(30.f);
			//ImGui::ColorEdit3("ParticleEffect", (float*)&pParticleComponent->GetColor()); // Edit 3 floats representing a color
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Afterimage Effect"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Damage Animation"))
		{
			DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(pCurrentAnimation->GetDamageAnimationComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##DamageAnimation", &pDamageAnimationComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("MaxDistance##DamageAnimation", &pDamageAnimationComponent->GetMaxDistance(), 0.01f, 0.0f, 10.0f, "%.2f", 0))
				pDamageAnimationComponent->GetMaxDistance() = std::clamp(pDamageAnimationComponent->GetMaxDistance(), 0.0f, 10.0f);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Speed##DamageAnimation", &pDamageAnimationComponent->GetSpeed(), 0.01f, 0.0f, 200.0f, "%.2f", 0))
				pDamageAnimationComponent->GetSpeed() = std::clamp(pDamageAnimationComponent->GetSpeed(), 0.0f, 200.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shake Animation"))
		{
			ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(pCurrentAnimation->GetShakeAnimationComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##ShakeAnimation", &pShakeAnimationComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Distance##ShakeAnimation", &pShakeAnimationComponent->GetDistance(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				pShakeAnimationComponent->GetDistance() = std::clamp(pShakeAnimationComponent->GetDistance(), 0.0f, 1.0f);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Frequency##ShakeAnimation", &pShakeAnimationComponent->GetFrequency(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				pShakeAnimationComponent->GetFrequency() = std::clamp(pShakeAnimationComponent->GetFrequency(), 0.0f, 1.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Stun Animation"))
		{
			StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(pCurrentAnimation->GetStunAnimationComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##StunAnimation", &pStunAnimationComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("StunTime##StunAnimation", &pStunAnimationComponent->GetStunTime(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				pStunAnimationComponent->GetStunTime() = std::clamp(pStunAnimationComponent->GetStunTime(), 0.0f, 1.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera Move"))
		{
			CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(pCurrentAnimation->GetCameraMoveComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##Move", &pCameraMoveComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Distance##Move", &pCameraMoveComponent->GetMaxDistance(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Time##Move", &pCameraMoveComponent->GetMovingTime(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("RollBackTime##Move", &pCameraMoveComponent->GetRollBackTime(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera Shake"))
		{
			CameraShakeComponent* pCameraShakerComponent = dynamic_cast<CameraShakeComponent*>(pCurrentAnimation->GetCameraShakeComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##Shake", &pCameraShakerComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Magnitude##Shake", &pCameraShakerComponent->GetMagnitude(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Duration##Shake", &pCameraShakerComponent->GetDuration(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera ZoomIn/ZoomOut"))
		{
			CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(pCurrentAnimation->GetCameraZoomerComponent());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##Zoom", &pCameraZoomerComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Distance##Zoom", &pCameraZoomerComponent->GetMaxDistance(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Time##Zoom", &pCameraZoomerComponent->GetMovingTime(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("RollBackTime##Zoom", &pCameraZoomerComponent->GetRollBackTime(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("IN / OUT##Zoom", &pCameraZoomerComponent->GetIsIn());
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shock Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOCK);
			
			SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShockSoundComponent());
			
			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##effectsound", &pShockSoundComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Combo("Sound##effectsound", (int*)&pShockSoundComponent->GetSoundNumber(), items.data(), items.size());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Delay##effectsound", &pShockSoundComponent->GetDelay(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Volume##effectsound", &pShockSoundComponent->GetVolume(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shooting Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOOT);

			SoundPlayComponent* pShootSoundComponent = dynamic_cast<SoundPlayComponent*>(pCurrentAnimation->GetShootSoundComponent());

			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##shootsound", &pShootSoundComponent->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Combo("Sound##shootsound", (int*)&pShootSoundComponent->GetSoundNumber(), items.data(), items.size());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Delay##shootsound", &pShootSoundComponent->GetDelay(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Volume##shootsound", &pShootSoundComponent->GetVolume(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Damage Moan Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_VOICE);
			
			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			/*initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##damagesound", &damage->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Combo("Sound##damagesound", (int*)&damage->m_nSoundNumber, items.data(), items.size());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Delay##damagesound", &damage->m_fDelay, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Volume##damagesound", &damage->m_fVolume, 0.01f, 0.0f, 10.0f, "%.2f", 0);*/
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);
		if (ImGui::CollapsingHeader("Attack Sprite Effect"))
		{
			//일단 보류
			/*CAttackSpriteComponent* AttackSprite = (CAttackSpriteComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CAttackSpriteComponent)));

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("On/Off##SpriteEffect", &AttackSprite->m_vSprite[0].second->GetAnimation());

			std::vector<std::shared_ptr<CTexture>> vTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetBillBoardTextureList();
			std::vector<const char*> items;
			std::vector <std::string> str(100);
			for (int i = 0; i < vTexture.size(); i++)
			{
				std::wstring wstr = vTexture[i]->GetTextureName(0);
				str[i].assign(wstr.begin(), wstr.end());
				items.emplace_back(str[i].c_str());
			}

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			int iAttackSpriteN = AttackSprite->GetAttackNumber();
			ImGui::Combo("##Attack", (int*)(&AttackSprite->GetAttackNumber()), items.data(), items.size());
			if(iAttackSpriteN != AttackSprite->GetAttackNumber())
				AttackSprite->SetTexture(0, ConverCtoWC(items[AttackSprite->GetAttackNumber()]));

			initial_curpos.x += 200.f;
			ImGui::SetCursorPos(initial_curpos);
			if (ImGui::Button("Animation"))
				AttackSprite->SetSpriteEnable(0);
			initial_curpos.x -= 200.f;

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Speed", &AttackSprite->GetSpeed(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Alpha", &AttackSprite->GetAlpha(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
			AttackSprite->UpdateData();*/
		}

		
		button_pos.y += 5.f;
		ImGui::SetCursorPos(button_pos);

		if (ImGui::Button("Animation1", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(0);
			Player_Animation_Number = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Animation2", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(1);
			Player_Animation_Number = 1;

		}
		ImGui::SameLine();
		if (ImGui::Button("Animation3", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CSimulatorScene::GetInst()->SetPlayerAnimationSet(2);
			Player_Animation_Number = 2;
		}
		ImGui::End();
	}
}
void CImGuiManager::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	::SynchronizeResourceTransition(pd3dCommandList, m_pRTTexture->GetResource(0), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//PrepareRenderTarget(pd3dCommandList, d3dDsvDescriptorCPUHandle);
	CSimulatorScene::GetInst()->OnPrepareRender(pd3dCommandList);

	CSimulatorScene::GetInst()->OnPreRender(pd3dCommandList, fTimeElapsed);

	PrepareRenderTarget(pd3dCommandList, d3dDsvDescriptorCPUHandle);

	CSimulatorScene::GetInst()->Render(pd3dCommandList, fTimeElapsed, fCurrentTime, pCamera);

	::SynchronizeResourceTransition(pd3dCommandList, m_pRTTexture->GetResource(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
}
void CImGuiManager::PrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle)
{
	FLOAT pfDefaultClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pd3dCommandList->ClearDepthStencilView(*d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	pd3dCommandList->ClearRenderTargetView(m_pd3dRtvCPUDescriptorHandles, pfDefaultClearColor, 0, NULL);
	pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvCPUDescriptorHandles, FALSE, d3dDsvDescriptorCPUHandle);
}
void CImGuiManager::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Rendering
	ImGui::Render();

	pd3dCommandList->SetDescriptorHeaps(1, m_pd3dSrvDescHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pd3dCommandList);
}

void CImGuiManager::OnPostRender()
{
	CSimulatorScene::GetInst()->OnPostRender();
}

void CImGuiManager::OnDestroy()
{
	m_pDataLoader->SaveComponentSets();
}

