#include "LobbyScene.h"
#include "SimulatorScene.h"
#include "..\Global\Global.h"
#include "..\Global\Locator.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Shader\UIObjectShader.h"
#include "..\Object\UIObject.h"
#include "..\Shader\ModelShader.h"
#include "../Sound/SoundManager.h"
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
	POINT ptCursorPos;
	float cxDelta, cyDelta;
	GetCursorPos(&ptCursorPos);
	m_ptOldCursorPos = ptCursorPos;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		// 좌클릭시 사용자가 좌클릭 했음을 표현하는 변수를 true로 바꿔줌

		if (dynamic_cast<CButtonObject*>(m_pUIObject[5].get())->CheckCollisionMouse(ptCursorPos))
		{
			CSoundManager::GetInst()->PlaySound("Sound/UI/Metal Click.wav", 3.f, 0.f);
			m_iSceneType = LobbySceneType::SIMULATOR_Scene;
			CSoundManager::GetInst()->StopSound("Sound/Background/Logo Bgm.wav");
			return true;
		}

		if (dynamic_cast<CButtonObject*>(m_pUIObject[3].get())->CheckCollisionMouse(ptCursorPos))
		{
			CSoundManager::GetInst()->PlaySound("Sound/UI/Metal Click.wav", 3.f, 0.f);
			//m_iSceneType = LobbySceneType::SIMULATOR_Scene;
			//CSoundManager::GetInst()->StopSound("Sound/Background/Logo Bgm.wav");

			return true;
		}


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
		case VK_ESCAPE:
			if (m_iSceneType == LobbySceneType::SIMULATOR_Scene)
				CImGuiManager::GetInst()->SetShowSettingMenu(!CImGuiManager::GetInst()->GetShowSettingMenu());
		//case VK_F5:
		//	if(m_iSceneType == LobbySceneType::LOGO_Scene)
		//		return SCENE_RETURN_TYPE::POP_LOGOSCENE;
		//	else
		//		return SCENE_RETURN_TYPE::POP_SIMULATORSCENE;
		//default:
		//	break;
		}
	case VK_BACK:
		//if(m_iSceneType == LobbySceneType::SIMULATOR_Scene)
		//	return SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE;
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

	// Light 생성
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);


	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);

	XMFLOAT3 offset{ -86.4804 , 0.0f, -183.7856 };

	m_pMap = std::make_unique<CMap>();
	m_pMap->Init(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	m_pMap->GetTerrain()->SetPosition(XMFLOAT3(offset.x, 0, offset.z));


	m_pUIObjectShader = std::make_unique<CUIObjectShader>();
	m_pUIObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	m_pTextureManager = std::make_unique<CTextureManager>();
	m_pTextureManager->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 100);
	m_pTextureManager->LoadSphereBuffer(pd3dDevice, pd3dCommandList);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Logo2.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/GameStart.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/GameExit.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/LogoName.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/LogoMenuFrame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/LogoMenuFrame2.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/LogoBackGround.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ResultFrame.dds", 0, 0);

	m_pTextureManager->CreateResourceView(pd3dDevice, 0);

	m_pUIObjectShader = std::make_unique<CUIObjectShader>();
	m_pUIObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	m_pSunLightShader = std::make_unique<CSunLightShader>();
	m_pSunLightShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	std::shared_ptr<CTexture> pSkyBoxTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/Epic_BlueSunset.dds", RESOURCE_TEXTURE_CUBE, 0);

	m_pSkyBoxShader = std::make_unique<CSkyBoxShader>();
	m_pSkyBoxShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSkyBoxShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1);
	m_pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture.get(), 0, 10);

	m_pSkyBoxObject = std::make_unique<CSkyBox>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), pSkyBoxTexture);

	std::unique_ptr<CUIObject> pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.5f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoBackGround.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(378.f * 2.5f, 283.f * 2.5f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.31f, FRAME_BUFFER_HEIGHT * 0.78f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoName.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	//pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	//pUIObject->SetSize(XMFLOAT2(251.f * 1.95f, 254.f * 1.6f));
	//pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f - 10.f, FRAME_BUFFER_HEIGHT * 0.35f));
	//pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoMenuFrame.dds"));
	//m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(762.f * 0.85f, 173.f * 1.f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f - 10.f, FRAME_BUFFER_HEIGHT * 0.25f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoMenuFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CColorButtonObject>(2, pd3dDevice, pd3dCommandList, 10.f); // 3
	pUIObject->SetSize(XMFLOAT2(762.f * 0.85f, 173.f * 1.f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f - 10.f, FRAME_BUFFER_HEIGHT * 0.25f));
	pUIObject->SetAlpha(0.f);
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoMenuFrame2.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(762.f * 1.f, 173.f * 0.9f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.45f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoMenuFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CColorButtonObject>(2, pd3dDevice, pd3dCommandList, 10.f); // 5
	pUIObject->SetSize(XMFLOAT2(762.f * 1.f, 173.f * 0.9f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.45f));
	pUIObject->SetAlpha(0.f);
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/LogoMenuFrame2.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(600.f * 0.7f, 120.f * 0.7f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.45f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/GameStart.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(600.f * 0.7f, 120.f * 0.7f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.25f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/GameExit.dds"));
	m_pUIObject.push_back(std::move(pUIObject));


	

	//pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	//pUIObject->SetSize(XMFLOAT2(1890.f * 0.34f, 1417.f * 0.34f));
	//pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.8f));
	//pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Logo2.dds"));
	//m_pUIObject.push_back(std::move(pUIObject));

	m_pCamera = std::make_unique<CFloatingCamera>();
	m_pCamera->Init(pd3dDevice, pd3dCommandList);
	m_pCamera->SetPosition(XMFLOAT3(0.0f, 400.0f, -100.0f));

	m_pCamera->SetUpdatedContext(m_pMap.get());

	CSoundManager::GetInst()->PlaySound("Sound/Background/Logo Bgm.wav", 1.f, 0.5f);
}

void CLobbyScene::Update(float fTimeElapsed)
{

	switch (m_iSceneType)
	{
	case LobbySceneType::LOGO_Scene:
		dynamic_cast<CButtonObject*>(m_pUIObject[3].get())->CheckCollisionMouse(m_ptOldCursorPos);
		dynamic_cast<CButtonObject*>(m_pUIObject[5].get())->CheckCollisionMouse(m_ptOldCursorPos);
		CSimulatorScene::GetInst()->Update(fTimeElapsed);
		return;
	case LobbySceneType::SIMULATOR_Scene:
		CSimulatorScene::GetInst()->Update(fTimeElapsed);
		if (CImGuiManager::GetInst()->GetChangeAfterScene())
		{
			m_iSceneType = LobbySceneType::Main_Scene;
			CImGuiManager::GetInst()->GetChangeAfterScene() = false;
		}
		else if (CImGuiManager::GetInst()->GetChangeBeforeScene())
		{
			m_iSceneType = LobbySceneType::LOGO_Scene;
			CSoundManager::GetInst()->PlaySound("Sound/Background/Logo Bgm.wav", 1.f, 0.5f);
			CImGuiManager::GetInst()->GetChangeBeforeScene() = false;
		}
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

		m_pLight->Render(pd3dCommandList);

		m_pSunLightShader->Render(pd3dCommandList, m_pCamera.get());


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
