#include "LobbyScene.h"
#include "SimulatorScene.h"
#include "..\Global\Global.h"
#include "..\Global\Locator.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Shader\UIObjectShader.h"
#include "..\Object\UIObject.h"
void CLobbyScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
		return;
	case LobbySceneType::SIMULATOR_Scene:
		CImGuiManager::GetInst()->SetUI();
		return;
	}
}
void CLobbyScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
}


bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		// 좌클릭시 사용자가 좌클릭 했음을 표현하는 변수를 true로 바꿔줌
		POINT ptCursorPos;
		float cxDelta, cyDelta;
		GetCursorPos(&ptCursorPos);
		if (dynamic_cast<CButtonObject*>(m_pUIObject[1].get())->CheckCollisionMouse(ptCursorPos))
			m_iSceneType = LobbySceneType::SIMULATOR_Scene;
		break;
	case WM_RBUTTONDOWN:

		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

	return 0;
}

SCENE_RETURN_TYPE CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, DWORD& dwDirection)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F5:
			if(m_iSceneType == LobbySceneType::LOGO_Scene)
				return SCENE_RETURN_TYPE::POP_LOGOSCENE;
			else
				return SCENE_RETURN_TYPE::POP_SIMULATORSCENE;
		default:
			break;
		}
	case VK_BACK:
		if(m_iSceneType == LobbySceneType::SIMULATOR_Scene)
			return SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE;
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		default:
			break;
		}
	default:
		break;
	}
	return SCENE_RETURN_TYPE::NONE;
}


void CLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{


	CSimulatorScene::GetInst()->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pd3dGraphicsRootSignature = CSimulatorScene::GetInst()->GetGraphicsRootSignature();
	m_pd3dComputeRootSignature = CSimulatorScene::GetInst()->GetComputeRootSignature();
	m_pHDRComputeShader = CSimulatorScene::GetInst()->m_pHDRComputeShader;
	m_pBloomComputeShader = CSimulatorScene::GetInst()->m_pBloomComputeShader;
	m_pPostProcessShader = CSimulatorScene::GetInst()->m_pPostProcessShader;
	/*m_pHDRComputeShader = std::make_unique<CHDRComputeShader>();
	m_pHDRComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());

	m_pBloomComputeShader = std::make_unique<CBloomShader>();
	m_pBloomComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());
	m_pBloomComputeShader->CreateBloomUAVResource(pd3dDevice, pd3dCommandList, 1920, 1080);*/

	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	m_pUIObjectShader = std::make_unique<CUIObjectShader>();
	m_pUIObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	m_pTextureManager = std::make_unique<CTextureManager>();
	m_pTextureManager->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 100);
	m_pTextureManager->LoadSphereBuffer(pd3dDevice, pd3dCommandList);
	m_pTextureManager->LoadTexture(TextureType::SmokeTexture, pd3dDevice, pd3dCommandList, L"Image/SmokeImages/Smoke2.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Explode_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Fire_Effect.dds", 5, 6);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Circle1.dds", 1, 1);

	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/TextureFlash2.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Meteor.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Effect0.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_11.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_21.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_4.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_14.dds", 1, 1);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Frame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Hp.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Mana.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Tonus.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Mana2.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Number/Numbers.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/BloodEffect.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Logo.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Button.dds", 0, 0);
	m_pTextureManager->CreateResourceView(pd3dDevice, 0);

	m_pUIObjectShader = std::make_unique<CUIObjectShader>();
	m_pUIObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);



	std::unique_ptr<CUIObject> pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.5f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Logo.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CButtonObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.12f, FRAME_BUFFER_HEIGHT * 0.1f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.9f, FRAME_BUFFER_HEIGHT * 0.9f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Button.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	//pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	//pUIObject->SetSize(XMFLOAT2(426 * 0.65f, 42 * 0.65f));
	//pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.2f + 80.f, FRAME_BUFFER_HEIGHT * 0.8f + 25.f));
	//pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Hp.dds"));
	//m_pUIObject.push_back(std::move(pUIObject));

	m_pCamera = CSimulatorScene::GetInst()->GetCamera();
}

void CLobbyScene::Update(float fTimeElapsed)
{
	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:
		CSimulatorScene::GetInst()->Update(fTimeElapsed);
		return;
	case LobbySceneType::SIMULATOR_Scene:
		CSimulatorScene::GetInst()->Update(fTimeElapsed);
		return;
	}
}

void CLobbyScene::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:
		m_pPostProcessShader->OnPrepareRenderTarget(pd3dCommandList, 1, pd3dRtvCPUHandles, d3dDepthStencilBufferDSVCPUHandle);
		return;
	case LobbySceneType::SIMULATOR_Scene:
		return;
	}

}

void CLobbyScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:

		m_pCamera->OnPrepareRender(pd3dCommandList);

		m_pTextureManager->SetTextureDescriptorHeap(pd3dCommandList);
		m_pTextureManager->UpdateShaderVariables(pd3dCommandList);

		m_pUIObjectShader->Render(pd3dCommandList, 0);
		for (int i = 0; i < m_pUIObject.size(); i++)
		{
			m_pUIObject[i]->Update(fTimeElapsed);
			m_pUIObject[i]->Render(pd3dCommandList, false, nullptr);
		}
		return;
	case LobbySceneType::SIMULATOR_Scene:
		CImGuiManager::GetInst()->Render(pd3dCommandList);
		return;
	}


}

void CLobbyScene::OnPostRender()
{
	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:
		CSimulatorScene::GetInst()->OnPostRender();
		return;
	case LobbySceneType::SIMULATOR_Scene:
		CSimulatorScene::GetInst()->OnPostRender();
		return;
	}

}

bool CLobbyScene::ProcessInput(HWND hWnd, DWORD dwDirection, float fTimeElapsed)
{
	return false;
}
