#include "ImGuiManager.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Scene\SimulatorScene.h"
#include "..\Object\Texture.h"
#include "..\Global\GameFramework.h"
#include "..\Global\Locator.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundPlayer.h"
#include "..\Sound\SoundManager.h"
#include "..\Object\BillBoardComponent.h"
#include "..\Object\TextureManager.h"
#include "..\Object\ParticleComponent.h"
#include "..\Object\AnimationComponent.h"

#define NUM_FRAMES_IN_FLIGHT 3

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

	Locator.CreateSimulatorCamera(pd3dDevice, pd3dCommandList);
	m_pCamera = Locator.GetSimulaterCamera();

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

	if (m_pCurrentComponentSet == nullptr) {
		m_pCurrentComponentSet = Locator.GetComponentSet(0);
		m_pCamera->LoadComponentFromSet(m_pCurrentComponentSet);
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
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			CParticleComponent* pParticleComponent = (CParticleComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CParticleComponent)));

			ImGui::Checkbox("On/Off##ParticleEffect", &pParticleComponent->GetEnable());

			std::vector<std::shared_ptr<CTexture>> vTexture = CTextureManager::GetInst()->GetParticleTextureList();
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

			int iParticleSpriteN = pParticleComponent->GetParticleIndex();
			ImGui::Combo("Texture##ParticleEffect", (int*)(&pParticleComponent->GetParticleIndex()), items.data(), items.size());
			if (iParticleSpriteN != pParticleComponent->GetParticleIndex())
				pParticleComponent->SetTexture(0, ConverCtoWC(items[pParticleComponent->GetParticleIndex()]));

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Size##ParticleEffect", &pParticleComponent->m_fSize, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Alpha##ParticleEffect", &pParticleComponent->m_fAlpha, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("LifeTime##ParticleEffect", &pParticleComponent->m_fLifeTime, 0.01f, 0.0f, 10.0f, "%.1f", 0);

			//initial_curpos.y += 25.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(190.f);
			//ImGui::DragInt("ParticleCount##ParticleEffect", &pParticleComponent->m_nParticleNumber, 0.01f, 0.0f, 10.0f, "%d", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Speed##ParticleEffect", &pParticleComponent->m_fSpeed, 0.01f, 0.0f, 10.0f, "%.1f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(30.f);
			ImGui::ColorEdit3("Color##ParticleEffect", (float*)&pParticleComponent->m_f3Color); // Edit 3 floats representing a color
			//initial_curpos.x += 35.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(30.f);
			//ImGui::DragFloat("Color-G##ParticleEffect", &pParticleComponent->m_f3Color.y, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			//initial_curpos.x += 35.f;
			//ImGui::SetCursorPos(initial_curpos);
			//ImGui::SetNextItemWidth(30.f);
			//ImGui::DragFloat("Color-B##ParticleEffect", &pParticleComponent->m_f3Color.z, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			pParticleComponent->Update();
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
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CDamageAnimationComponent* damage = (CDamageAnimationComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CDamageAnimationComponent)));
			ImGui::Checkbox("On/Off##DamageAnimation", &damage->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("MaxDistance##DamageAnimation", &damage->GetMaxDistance(), 0.01f, 0.0f, 10.0f, "%.2f", 0))
				damage->GetMaxDistance() = std::clamp(damage->GetMaxDistance(), 0.0f, 10.0f);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Speed##DamageAnimation", &damage->GetSpeed(), 0.01f, 0.0f, 200.0f, "%.2f", 0))
				damage->GetSpeed() = std::clamp(damage->GetSpeed(), 0.0f, 200.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shake Animation"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CShakeAnimationComponent* shake = (CShakeAnimationComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CShakeAnimationComponent)));
			ImGui::Checkbox("On/Off##ShakeAnimation", &shake->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Distance##ShakeAnimation", &shake->GetDistance(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				shake->GetDistance() = std::clamp(shake->GetDistance(), 0.0f, 1.0f);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("Frequency##ShakeAnimation", &shake->GetFrequency(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				shake->GetFrequency() = std::clamp(shake->GetFrequency(), 0.0f, 1.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Stun Animation"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CStunAnimationComponent* stun = (CStunAnimationComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CStunAnimationComponent)));
			ImGui::Checkbox("On/Off##StunAnimation", &stun->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			if (ImGui::DragFloat("StunTime##StunAnimation", &stun->GetStunTime(), 0.01f, 0.0f, 1.0f, "%.2f", 0))
				stun->GetStunTime() = std::clamp(stun->GetStunTime(), 0.0f, 1.0f);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		CCamera* pCamera = Locator.GetSimulaterCamera();

		if (ImGui::CollapsingHeader("Camera Move"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CCameraMover* mover = (CCameraMover*)(m_pCurrentComponentSet->FindComponent(typeid(CCameraMover)));
			ImGui::Checkbox("On/Off##Move", &mover->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Distance##Move", &mover->m_fMaxDistance, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Time##Move", &mover->m_fMovingTime, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("RollBackTime##Move", &mover->m_fRollBackTime, 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera Shake"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CCameraShaker* shaker = (CCameraShaker*)(m_pCurrentComponentSet->FindComponent(typeid(CCameraShaker)));
			ImGui::Checkbox("On/Off##Shake", &shaker->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Magnitude##Shake", &shaker->m_fMagnitude, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Duration##Shake", &shaker->m_fDuration, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Camera ZoomIn/ZoomOut"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CCameraZoomer* Zoomer = (CCameraZoomer*)(m_pCurrentComponentSet->FindComponent(typeid(CCameraZoomer)));
			ImGui::Checkbox("On/Off##Zoom", &Zoomer->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			ImGui::DragFloat("Distance##Zoom", &Zoomer->m_fMaxDistance, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Time##Zoom", &Zoomer->m_fMovingTime, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("RollBackTime##Zoom", &Zoomer->m_fRollBackTime, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Checkbox("IN / OUT##Zoom", &Zoomer->m_bIsIN);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shock Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOCK);
			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CEffectSoundComponent* effect = (CEffectSoundComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CEffectSoundComponent)));
			ImGui::Checkbox("On/Off##effectsound", &effect->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Combo("Sound##effectsound", (int*)&effect->m_nSoundNumber, items.data(), items.size());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Delay##effectsound", &effect->m_fDelay, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Volume##effectsound", &effect->m_fVolume, 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);

		if (ImGui::CollapsingHeader("Shooting Sound Effect"))
		{
			std::vector<std::string> paths = CSoundManager::GetInst()->getSoundPathsByCategory(SOUND_CATEGORY::SOUND_SHOOT);
			std::vector<const char*> items;
			for (auto& path : paths) {
				items.push_back(path.c_str());
			}

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CShootSoundComponent* shoot = (CShootSoundComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CShootSoundComponent)));
			ImGui::Checkbox("On/Off##shootsound", &shoot->GetEnable());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::Combo("Sound##shootsound", (int*)&shoot->m_nSoundNumber, items.data(), items.size());

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Delay##shootsound", &shoot->m_fDelay, 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Volume##shootsound", &shoot->m_fVolume, 0.01f, 0.0f, 10.0f, "%.2f", 0);

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

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			CDamageSoundComponent* damage = (CDamageSoundComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CDamageSoundComponent)));
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
			ImGui::DragFloat("Volume##damagesound", &damage->m_fVolume, 0.01f, 0.0f, 10.0f, "%.2f", 0);
		}

		/* initial_curpos.y += 25.f;
		 ImGui::SetCursorPos(initial_curpos);
		 ImGui::SetNextItemWidth(170.f);
		 ImGui::DragFloat("Parallax Scale", (float*)&ParallaxScale, 0.01f, 0.0f, 1.0f, "%.6f", 0);

		 initial_curpos.y += 25.f;
		 ImGui::SetCursorPos(initial_curpos);
		 ImGui::SetNextItemWidth(170.f);
		 ImGui::DragFloat("Parallax Bias", (float*)&ParallaxBias, 0.01f, 0.0f, 1.0f, "%.6f", 0);

		 initial_curpos.y += 25.f;
		 ImGui::SetCursorPos(initial_curpos);
		 ImGui::SetNextItemWidth(120.f);
		 ImGui::InputInt("Terrain Mapping Mode", (int*)&Terrain_Mapping_mode, 1, 1, 0);*/

		initial_curpos.y += 25.f;
		ImGui::SetCursorPos(initial_curpos);
		if (ImGui::CollapsingHeader("Attack Sprite Effect"))
		{
			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);

			CAttackSpriteComponent* AttackSprite = (CAttackSpriteComponent*)(m_pCurrentComponentSet->FindComponent(typeid(CAttackSpriteComponent)));

			ImGui::Checkbox("On/Off##SpriteEffect", &AttackSprite->m_vSprite[0].second->GetAnimation());

			std::vector<std::shared_ptr<CTexture>> vTexture = CTextureManager::GetInst()->GetBillBoardTextureList();
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
			if (ImGui::Button("Animation##SpriteEffect"))
				AttackSprite->SetSpriteEnable(0);
			initial_curpos.x -= 200.f;

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Speed##SpriteEffect", &AttackSprite->GetSpeed(), 0.01f, 0.0f, 10.0f, "%.2f", 0);

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Alpha##SpriteEffect", &AttackSprite->GetAlpha(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
			AttackSprite->UpdateData();

			initial_curpos.y += 25.f;
			ImGui::SetCursorPos(initial_curpos);
			ImGui::SetNextItemWidth(190.f);
			ImGui::DragFloat("Size##SpriteEffect", &AttackSprite->GetSize(), 0.01f, 0.0f, 10.0f, "%.2f", 0);
			AttackSprite->UpdateData();
		}

		
		button_pos.y += 5.f;
		ImGui::SetCursorPos(button_pos);

		if (ImGui::Button("Animation1", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CComponentSet* componentset = Locator.GetComponentSet(0);
			if (componentset) {
				m_pCurrentComponentSet = componentset;
				m_pCamera->LoadComponentFromSet(m_pCurrentComponentSet);
				Locator.GetSoundPlayer()->LoadComponentFromSet(m_pCurrentComponentSet);
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Animation2", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CComponentSet* componentset = Locator.GetComponentSet(1);
			if (componentset) {
				m_pCurrentComponentSet = componentset;
				m_pCamera->LoadComponentFromSet(m_pCurrentComponentSet);
				Locator.GetSoundPlayer()->LoadComponentFromSet(m_pCurrentComponentSet);
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Animation3", ImVec2(175.f, 45.f))) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			CComponentSet* componentset = Locator.GetComponentSet(2);
			if (componentset) {
				m_pCurrentComponentSet = componentset;
				m_pCamera->LoadComponentFromSet(m_pCurrentComponentSet);
				Locator.GetSoundPlayer()->LoadComponentFromSet(m_pCurrentComponentSet);
				CSimulatorScene::GetInst()->SetPlayerAnimationSet(2);
			}
		}

		ImGui::End();
	}
}
void CImGuiManager::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE* d3dDsvDescriptorCPUHandle, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	::SynchronizeResourceTransition(pd3dCommandList, m_pRTTexture->GetResource(0), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//PrepareRenderTarget(pd3dCommandList, d3dDsvDescriptorCPUHandle);
	CSimulatorScene::GetInst()->OnPrepareRender(pd3dCommandList);

	/*if (pCamera)
		pCamera->OnPrepareRender(pd3dCommandList);*/
	CSimulatorScene::GetInst()->OnPreRender(pd3dCommandList, fTimeElapsed);

	//PrepareRenderTarget(pd3dCommandList, d3dDsvDescriptorCPUHandle);
	CSimulatorScene::GetInst()->OnPrepareRenderTarget(pd3dCommandList, 1, &m_pd3dRtvCPUDescriptorHandles, *d3dDsvDescriptorCPUHandle);
	/*if (pCamera)
		pCamera->OnPrepareRender(pd3dCommandList);*/
	m_pCamera->Animate(fTimeElapsed);
	m_pCamera->RegenerateViewMatrix();
	m_pCamera->OnPrepareRender(pd3dCommandList);

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

void CImGuiManager::OnPostRenderTarget()
{
	CSimulatorScene::GetInst()->OnPostRenderTarget();
}

