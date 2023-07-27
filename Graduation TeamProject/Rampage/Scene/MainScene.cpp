#include "MainScene.h"
#include "..\Global\Timer.h"
#include "..\Global\Camera.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\Texture.h"
#include "..\Object\Player.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\ModelShader.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Shader\ParticleShader.h"
#include "..\Shader\DepthRenderShader.h"
#include "..\Object\Monster.h"
#include "..\Object\TextureManager.h"
#include "..\Object\PlayerParticleObject.h"
#include "..\Shader\UIObjectShader.h"
#include "..\Object\UIObject.h"
#include "SimulatorScene.h"
#include "..\Sound\SoundManager.h"
#include <PxForceMode.h>

#define MAX_GOBLIN_NUM		50
#define MAX_ORC_NUM			50
#define MAX_SKELETON_NUM	50

XMFLOAT3 RandomMonsterPos(XMFLOAT3 xmf3CenterPos, float fSpawnRange) {
	return XMFLOAT3{ xmf3CenterPos.x + RandomFloatInRange(-fSpawnRange, fSpawnRange),
				xmf3CenterPos.y,
				xmf3CenterPos.z + RandomFloatInRange(-fSpawnRange, fSpawnRange) };
}

CMainTMPScene::CMainTMPScene()
{
	std::unique_ptr<RegisterArticulationListener> listener = std::make_unique<RegisterArticulationListener>();
	listener->SetScene(this);
	listener->SetEnable(true);
	m_pListeners.push_back(std::move(listener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::REQUEST_REGISTERARTI, m_pListeners.back().get(), nullptr);


	std::unique_ptr<RegisterArticulationSleepListener> slistener = std::make_unique<RegisterArticulationSleepListener>();
	slistener->SetScene(this);
	slistener->SetEnable(true);
	m_pListeners.push_back(std::move(slistener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::REQUEST_SLEEPARTI, m_pListeners.back().get(), nullptr);

	// COLLIDE LISTENER
	std::unique_ptr<SceneCollideListener> pCollideListener = std::make_unique<SceneCollideListener>();
	pCollideListener->SetScene(this);
	m_pListeners.push_back(std::move(pCollideListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::COLLISION, m_pListeners.back().get(), nullptr);

	// ONGROUND LISTENER
	std::unique_ptr<SceneOnGroundListener> pOnGroundListener = std::make_unique<SceneOnGroundListener>();
	pOnGroundListener->SetScene(this);
	m_pListeners.push_back(std::move(pOnGroundListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::ONGROUND, m_pListeners.back().get(), nullptr);

	// Monster Dead Listener
	std::unique_ptr<MonsterDeadListener> pMonsterDeadListener = std::make_unique<MonsterDeadListener>();
	pMonsterDeadListener->SetScene(this);
	m_pListeners.push_back(std::move(pMonsterDeadListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::MONSTER_DEAD, m_pListeners.back().get(), nullptr);

	// Cinematic All Updated Listener
	std::unique_ptr<CinematicAllUpdatedListener> pCinematicAllUpdatedListener = std::make_unique<CinematicAllUpdatedListener>();
	pCinematicAllUpdatedListener->SetScene(this);
	m_pListeners.push_back(std::move(pCinematicAllUpdatedListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::CINEMATIC_ALL_UPDATED, m_pListeners.back().get(), nullptr);

	// Player Dead Listener
	std::unique_ptr<PlayerDeadListener> pPlayerDeadListener = std::make_unique<PlayerDeadListener>();
	pPlayerDeadListener->SetScene(this);
	m_pListeners.push_back(std::move(pPlayerDeadListener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAYER_DEAD, m_pListeners.back().get(), nullptr);
}

void CMainTMPScene::HandleMonsterDeadMessage()
{
	m_iTotalMonsterNum -= 1;

	OutputDebugString(std::to_wstring(m_iTotalMonsterNum).c_str());
	OutputDebugString(L"\n");

	if (m_iTotalMonsterNum == 0)
	{
		if (m_iStageNum < m_vCinematicCameraLocations.size())
		{
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->ClearCameraInfo();
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->AddPlayerCameraInfo((CPlayer*)m_pPlayer, m_pMainSceneCamera.get());
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->AddCameraInfo(m_vCinematicCameraLocations[m_iStageNum].get());
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->PlayCinematicCamera();

			// 시작은 씨네마틱 카메라
			m_pCurrentCamera = m_pCinematicSceneCamera.get();
			m_curSceneProcessType = SCENE_PROCESS_TYPE::CINEMATIC;
		}

		else
			AdvanceStage();
	}
}

void CMainTMPScene::HandlePlayerDeadMessage()
{
	OutputDebugString(L"Player Dead\n");

	float fTotalGameTime = Locator.GetTimer()->GetTotalTime() - m_fGameStartTime;

	// 결과창 띄우기
	m_pUIObject[13]->SetEnable(true);
	dynamic_cast<CResultFrame*>(m_pUIObject[13].get())->SetResultData(((CPlayer*)m_pPlayer)->m_iCombo, fTotalGameTime, ((CPlayer*)m_pPlayer)->m_nRemainPotions);
}

void CMainTMPScene::AdvanceStage()
{
	m_iStageNum += 1;

	if (m_StageInfoMap.size() < m_iStageNum)
	{
		float fTotalGameTime = Locator.GetTimer()->GetTotalTime() - m_fGameStartTime;

		// 결과창 띄우기
		m_pUIObject[13]->SetEnable(true);
		dynamic_cast<CResultFrame*>(m_pUIObject[13].get())->SetResultData(((CPlayer*)m_pPlayer)->m_iCombo, fTotalGameTime, ((CPlayer*)m_pPlayer)->m_nRemainPotions);
	}

	StageInfo stageInfo = m_StageInfoMap.find(m_iStageNum - 1)->second;
	float fTotalHP = 0.0f;

	for (const SpawnInfo& stageSpawnInfo : stageInfo.m_vSpawnInfo)
	{
		// Spawn Goblin
		{

			// Spawn Normal Golbin
			m_iTotalMonsterNum += stageSpawnInfo.m_iGoblinNum;

			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iGoblinNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = false;
				}
				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::GOBLIN, stageSpawnInfo.m_iGoblinNum, xmf3MonsterSpawnInfo.data());
			}

			// Spawn Elite Goblin
			m_iTotalMonsterNum += stageSpawnInfo.m_iEliteGolbinNum;

			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iEliteGolbinNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = true;
				}
				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::GOBLIN, stageSpawnInfo.m_iEliteGolbinNum, xmf3MonsterSpawnInfo.data());
			}
		}

		// Spawn Orc
		{
			// Spawn Normal Orc
			m_iTotalMonsterNum += stageSpawnInfo.m_iOrcNum;

			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iOrcNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = false;
				}

				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::ORC, stageSpawnInfo.m_iOrcNum, xmf3MonsterSpawnInfo.data());
			}

			// Spawn Elite Orc
			m_iTotalMonsterNum += stageSpawnInfo.m_iEliteOrcNum;
			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iEliteOrcNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = true;
				}

				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::ORC, stageSpawnInfo.m_iEliteOrcNum, xmf3MonsterSpawnInfo.data());
			}
		}

		// Spawn Skeleton
		{
			// Spawn Normal Skeleton
			m_iTotalMonsterNum += stageSpawnInfo.m_iSkeletonNum;

			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iSkeletonNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = false;
				}

				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::SKELETON, stageSpawnInfo.m_iSkeletonNum, xmf3MonsterSpawnInfo.data());
			}

			// Spawn Elite Skeleton
			m_iTotalMonsterNum += stageSpawnInfo.m_iEliteSkeletonNum;
			{
				std::vector<MonsterSpawnInfo> xmf3MonsterSpawnInfo(stageSpawnInfo.m_iEliteSkeletonNum);

				for (MonsterSpawnInfo& spawnInfo : xmf3MonsterSpawnInfo)
				{
					spawnInfo.xmf3Position = RandomMonsterPos(stageSpawnInfo.m_xmf3StageCenterPos, stageSpawnInfo.m_fMonsterSpawnRange);
					spawnInfo.bIsElite = true;
				}

				CMonsterPool::GetInst()->SpawnMonster(MONSTER_TYPE::SKELETON, stageSpawnInfo.m_iEliteSkeletonNum, xmf3MonsterSpawnInfo.data());
			}
		}

		fTotalHP += (75.0f * stageSpawnInfo.m_iGoblinNum +
			75.0f * 2.0f * stageSpawnInfo.m_iEliteGolbinNum +
			200.0f * stageSpawnInfo.m_iOrcNum +
			200.0f * 2.0f * stageSpawnInfo.m_iEliteOrcNum +
			135.0f * stageSpawnInfo.m_iSkeletonNum +
			135.0f * 2.0f * stageSpawnInfo.m_iEliteSkeletonNum);
	}

	m_StageInfoMap.find(m_iStageNum - 1)->second.m_fTotalHP = fTotalHP;

	{
		m_pUIObject[6]->m_bEnable = true;
		m_pUIObject[7]->m_bEnable = true;

		dynamic_cast<CBarObject*>(m_pUIObject[7].get())->Set_Value(fTotalHP, fTotalHP);
	}

	m_curSceneProcessType = SCENE_PROCESS_TYPE::WAITING;
	m_fWaitingTime = 0.0f;
}

void CMainTMPScene::returnMainCamera()
{
	XMFLOAT3 xmf3PlayerPos = XMFLOAT3{
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
		 m_pPlayer->GetPosition().y,
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43 };
	xmf3PlayerPos.y += MeterToUnit(0.9f);


	XMFLOAT3 CinematicPos = m_pCinematicPlayerCamera->GetPosition();
	XMFLOAT3 distanceOffset = Vector3::Subtract(CinematicPos, xmf3PlayerPos);
	distanceOffset = XMFLOAT3(distanceOffset.x, 0, distanceOffset.z);
	distanceOffset = Vector3::Normalize(distanceOffset);
	XMFLOAT3 seekOffset = XMFLOAT3(0, 0, 1);

	XMFLOAT3 look = m_pCinematicPlayerCamera->GetLookVector();
	XMFLOAT3 right = m_pCinematicPlayerCamera->GetRightVector();
	XMFLOAT3 up = m_pCinematicPlayerCamera->GetUpVector();

	float pitch = Vector3::Angle(XMFLOAT3(0, 1, 0), up);

	m_pMainSceneCamera->SetPitch(0.0f);
	m_pMainSceneCamera->SetYaw(0.0f);

	m_pMainSceneCamera->Rotate(pitch);
	
	XMFLOAT3 MainCamOff = m_pMainSceneCamera->GetOffset();
	float distance = Vector3::Angle(Vector3::Normalize(XMFLOAT3(distanceOffset.x,0, distanceOffset.z)), Vector3::Normalize(XMFLOAT3(MainCamOff.x,0, MainCamOff.z)));
	float sign = Vector3::DotProduct(XMFLOAT3(0, 1, 0), Vector3::CrossProduct(Vector3::Normalize(XMFLOAT3(distanceOffset.x, 0, distanceOffset.z)), Vector3::Normalize(XMFLOAT3(MainCamOff.x, 0, MainCamOff.z)))) >= 0.0f ? 1.0f : -1.0f;
	while (distance > 0.1f)
	{
		m_pMainSceneCamera->Rotate(0.0f, 0.05f * sign);

		XMMATRIX xmmtxRotate = XMMatrixIdentity();
		XMMATRIX xmmtxRotateX = XMMatrixIdentity();
		XMMATRIX xmmtxRotateY = XMMatrixIdentity();
		XMMATRIX xmmtxRotateZ = XMMatrixIdentity();

		XMFLOAT3 xmf3XAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		XMFLOAT3 xmf3YAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMFLOAT3 xmf3ZAxis = XMFLOAT3(0.0f, 0.0f, 1.0f);


		if (m_pMainSceneCamera->GetYaw() != 0.0f)
		{
			xmmtxRotateY = XMMatrixRotationAxis(XMLoadFloat3(&xmf3YAxis), XMConvertToRadians(m_pMainSceneCamera->GetYaw()));
		}

		xmmtxRotate = XMMatrixMultiply(xmmtxRotateZ, XMMatrixMultiply(xmmtxRotateX, xmmtxRotateY));
		XMFLOAT3 xmf3Offset = Vector3::TransformNormal(m_pMainSceneCamera->GetOffset(), xmmtxRotate);

		distance = Vector3::Angle(Vector3::Normalize(XMFLOAT3(distanceOffset.x, 0, distanceOffset.z)), Vector3::Normalize(XMFLOAT3(xmf3Offset.x, 0, xmf3Offset.z)));
	}

	m_pMainSceneCamera->RegenerateViewMatrix();
	m_pCurrentCamera = m_pMainSceneCamera.get();
	m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::THIRD_FERSON_MODE;
	m_curSceneProcessType = SCENE_PROCESS_TYPE::NORMAL;
}

void CMainTMPScene::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
	((CPlayer*)m_pPlayer)->SetUpdatedContext(m_pMap.get());
	((CPlayer*)m_pPlayer)->SetCamera(m_pMainSceneCamera.get());
	((CThirdPersonCamera*)m_pMainSceneCamera.get())->SetPlayer((CPlayer*)m_pPlayer);

	if (m_pDepthRenderShader.get())
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(pPlayer);

	m_pCollisionChecker->RegisterObject(pPlayer);
}

void CMainTMPScene::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (m_pDepthRenderShader)
	{
		((CDepthRenderShader*)m_pDepthRenderShader.get())->PrepareShadowMap(pd3dCommandList, 0.0f);
		((CDepthRenderShader*)m_pDepthRenderShader.get())->UpdateDepthTexture(pd3dCommandList);
	}
}
void CMainTMPScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
}
void CMainTMPScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 8;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0 ~ t6: MappingTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 13;
	pd3dDescriptorRanges[1].BaseShaderRegister = 10; //t10 ~ t22: Mapping Base Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = MAX_DEPTH_TEXTURES * 2;
	pd3dDescriptorRanges[2].BaseShaderRegister = 23; //t23: DepthWrite Texture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 50;
	pd3dDescriptorRanges[3].BaseShaderRegister = 50; //t50: gtxtTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 7;
	pd3dDescriptorRanges[4].BaseShaderRegister = 35; //t35: gtxMultiRenderTargetTextures
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[15];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 35;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //GameObject
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; // b1 : Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 2; //Skinned Bone Offsets
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 3; //Skinned Bone Transforms
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 5; // b5 : Dissolve
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[8].Descriptor.ShaderRegister = 6; // b6 : ToLight
	pd3dRootParameters[8].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //Depth Buffer
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; // cbFrameworkInfo
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 9;
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[14].Descriptor.ShaderRegister = 8; // b8 : cbLensFlarePOsitions
	pd3dRootParameters[14].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[5];

	d3dSamplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].MipLODBias = 0;
	d3dSamplerDesc[0].MaxAnisotropy = 1;
	d3dSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[0].MinLOD = 0;
	d3dSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[0].ShaderRegister = 0;
	d3dSamplerDesc[0].RegisterSpace = 0;
	d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].MipLODBias = 0;
	d3dSamplerDesc[1].MaxAnisotropy = 1;
	d3dSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	d3dSamplerDesc[1].MinLOD = 0;
	d3dSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister = 1;
	d3dSamplerDesc[1].RegisterSpace = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	d3dSamplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[2].MipLODBias = 0.0f;
	d3dSamplerDesc[2].MaxAnisotropy = 1;
	d3dSamplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //D3D12_COMPARISON_FUNC_LESS
	d3dSamplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[2].MinLOD = 0;
	d3dSamplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[2].ShaderRegister = 2;
	d3dSamplerDesc[2].RegisterSpace = 0;
	d3dSamplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[3].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[3].MipLODBias = 0.0f;
	d3dSamplerDesc[3].MaxAnisotropy = 1;
	d3dSamplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[3].MinLOD = 0;
	d3dSamplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[3].ShaderRegister = 3;
	d3dSamplerDesc[3].RegisterSpace = 0;
	d3dSamplerDesc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[4].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc[4].MipLODBias = 0;
	d3dSamplerDesc[4].MaxAnisotropy = 1;
	d3dSamplerDesc[4].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[4].MinLOD = 0;
	d3dSamplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[4].ShaderRegister = 4;
	d3dSamplerDesc[4].RegisterSpace = 0;
	d3dSamplerDesc[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(d3dSamplerDesc);;
	d3dRootSignatureDesc.pStaticSamplers = d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;

	HRESULT hResult = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	hResult = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
void CMainTMPScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 50; //t50: Texture2D
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //u0: RWTexture2D
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[2].NumDescriptors = 5;
	pd3dDescriptorRanges[2].BaseShaderRegister = 5; //u5 ~ u9: RWTexture2D
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[3].NumDescriptors = 5;
	pd3dDescriptorRanges[3].BaseShaderRegister = 10; //u10 ~ u14: RWTexture2D
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 5;
	pd3dDescriptorRanges[4].BaseShaderRegister = 55; //t55 ~ 59: Texture2D
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[0].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Texture2D
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //RWTexture2D
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; // BloomFilterTextures
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3]; // BluredTextures
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4]; // BluredTextures
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[5].Constants.Num32BitValues = 4;
	pd3dRootParameters[5].Constants.ShaderRegister = 0; //Bloom Level
	pd3dRootParameters[5].Constants.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;

	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;

	HRESULT hResult = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	hResult = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dComputeRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
void CMainTMPScene::UpdateObjectArticulation()
{
	for (int i = 0; i < m_pEnemys.size(); ++i) {
		m_pEnemys[i]->updateArticulationMatrix();
	}

	m_pMap->UpdateObjectArticulation();
}
void CMainTMPScene::RequestRegisterArticulation(RegisterArticulationParams param)
{
	m_lRequestObjects.emplace_back(param);
}
void CMainTMPScene::RegisterArticulations()
{
	for (RegisterArticulationParams& param : m_lRequestObjects) {
		CGameObject* obj = param.pObject;
		bool flag = false;
		flag = Locator.GetPxScene()->addArticulation(*obj->m_pArticulation);
		obj->m_pArticulation->wakeUp();
		obj->m_pArticulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, false);

		int index = 0;
		for (XMFLOAT4X4& world : obj->m_AritculatCacheMatrixs) {
			physx::PxMat44 mat = obj->m_pArticulationLinks[index++]->getGlobalPose();

			memcpy(&world, &mat, sizeof(XMFLOAT4X4));
			XMFLOAT3 scale = ((CPhysicsObject*)obj)->GetScale();
			world = Matrix4x4::Multiply(XMMatrixScaling(scale.x, scale.y, scale.z), world);
		}

		if (flag) {
			obj->m_pArticulationCache = obj->m_pArticulation->createCache();
			obj->m_nArtiCache = obj->m_pArticulation->getCacheDataSize();
		}
		else {

		}

		obj->m_bSimulateArticulate = false;
		obj->Animate(0.0f);
		((CPhysicsObject*)obj)->UpdateTransform(NULL);
		XMFLOAT4X4 rootWorld = obj->FindFrame("root")->GetWorld();
		float _rootWorld[16] = {
			rootWorld._11,rootWorld._12,rootWorld._13,rootWorld._14,
			rootWorld._21,rootWorld._22,rootWorld._23,rootWorld._24,
			rootWorld._31,rootWorld._32,rootWorld._33,rootWorld._34,
			rootWorld._41,rootWorld._42,rootWorld._43,rootWorld._44,
		};
		obj->m_pArticulation->copyInternalStateToCache(*obj->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		physx::PxTransform* rootLInkTrans = &obj->m_pArticulationCache->rootLinkData->transform;
		physx::PxMat44 tobonetrans = physx::PxMat44(_rootWorld);

		*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
		obj->m_pArticulation->applyCache(*obj->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		obj->m_bSimulateArticulate = true;

		XMFLOAT3 force = param.m_force;
		obj->m_pArticulationLinks[1]->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eIMPULSE);
	}
	m_lRequestObjects.clear();
}
void CMainTMPScene::RequestSleepArticulation(RegisterArticulationSleepParams param)
{
	m_lSleepObjects.emplace_back(param);
}
void CMainTMPScene::SleepArticulations()
{
	for (RegisterArticulationSleepParams& param : m_lSleepObjects) {
		param.pObject->m_pArticulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, true);
		param.pObject->m_pArticulation->putToSleep();
		Locator.GetPxScene()->removeArticulation(*param.pObject->m_pArticulation);
	}
	m_lSleepObjects.clear();
}
bool CMainTMPScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		// 좌클릭시 사용자가 좌클릭 했음을 표현하는 변수를 true로 바꿔줌
		if (m_pPlayer)
		{
			if (!((CPlayer*)m_pPlayer)->m_bAttack)
			{
				((CPlayer*)m_pPlayer)->m_bAttack = true;
			}
		}
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
		break;
	default:
		break;
	}

	return 0;
}
SCENE_RETURN_TYPE CMainTMPScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	MOUSE_CUROSR_MODE eMouseMode = Locator.GetMouseCursorMode();

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_SHIFT:
			if (m_pPlayer)
			{
				if (!((CPlayer*)m_pPlayer)->m_bEvasioned && ((CPlayer*)m_pPlayer)->m_fStamina >= 20.0f)
				{
					((CPlayer*)m_pPlayer)->m_bEvasioned = true;
				}
			}
			break;
		case VK_CONTROL:
			if (m_pPlayer)
			{
				if (!((CPlayer*)m_pPlayer)->m_bIsDash)
				{
					((CPlayer*)m_pPlayer)->m_bIsDash = true;
					((CPlayer*)m_pPlayer)->SetSpeedUperS(((CPlayer*)m_pPlayer)->GetSpeedUperS() * 2.0f);
				}
			}
			break;
		case VK_TAB:
		case VK_F5:
			if (m_pCurrentCamera == m_pFloatingCamera.get()) {
				((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->AddCameraInfo(m_pCurrentCamera);
				((CCinematicCamera*)(m_pCinematicPlayerCamera.get()))->AddCameraInfo(m_pCurrentCamera);
			}

			break;
		case VK_F6:
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->ClearCameraInfo();
			break;
		case VK_F7:
			if (dynamic_cast<CDollyCamera*>(m_pCinematicSceneCamera.get()))
				dynamic_cast<CDollyCamera*>(m_pCinematicSceneCamera.get())->CaculateCubicPolyData();
			((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->PlayCinematicCamera();
			m_pCurrentCamera = m_pCinematicSceneCamera.get();
			m_curSceneProcessType = SCENE_PROCESS_TYPE::CINEMATIC;
			break;
		case VK_BACK:
			if (m_CurrentMouseCursorMode == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE)
			{
				while (m_iCursorHideCount > 0)
				{
					m_iCursorHideCount--;
					ShowCursor(true);
				}
				ClipCursor(NULL);
			}
			return SCENE_RETURN_TYPE::RETURN_PREVIOUS_SCENE;
		case '1':
		{
			m_pCurrentCamera = m_pFloatingCamera.get();
			m_curSceneProcessType = SCENE_PROCESS_TYPE::NORMAL;

			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::FLOATING_MODE);
			m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;

			while (m_iCursorHideCount > 0)
			{
				m_iCursorHideCount--;
				ShowCursor(true);
				ClipCursor(NULL);
			}
		}
		break;
		case '2':
		{
			m_pCurrentCamera = m_pMainSceneCamera.get();
			m_curSceneProcessType = SCENE_PROCESS_TYPE::NORMAL;

			Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::THIRD_FERSON_MODE);
			m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::THIRD_FERSON_MODE;
			PostMessage(hWnd, WM_ACTIVATE, 0, 0);

			if (m_iCursorHideCount < 1) {
				m_iCursorHideCount++;
				ShowCursor(false);
				ClipCursor(&m_ScreendRect);
			}
		}
		break;
		case '3':
			break;
		case 'f':
		case 'F':
			//((CPlayer*)m_pPlayer)->Tmp();
			break;
		case 'w':
		case 'W':
			if (wParam == 'w' || wParam == 'W') dwDirection |= DIR_FORWARD;
			break;
		case 's':
		case 'S':
			if (wParam == 's' || wParam == 'S') dwDirection |= DIR_BACKWARD;
			break;
		case 'a':
		case 'A':
			if (wParam == 'a' || wParam == 'A') dwDirection |= DIR_LEFT;
			break;
		case 'd':
		case 'D':
			if (wParam == 'd' || wParam == 'D') dwDirection |= DIR_RIGHT;
			break;
		case 'q':
		case 'Q':
			if (wParam == 'q' || wParam == 'Q')dwDirection |= DIR_DOWN;
			break;
		case 'e':
		case 'E':
			if (wParam == 'e' || wParam == 'E') {
				if (m_CurrentMouseCursorMode == MOUSE_CUROSR_MODE::FLOATING_MODE)
					dwDirection |= DIR_UP;
				else if (m_CurrentMouseCursorMode == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE) {
					if (m_pPlayer) {
						CStateMachine<CPlayer>* stateMachine = dynamic_cast<CStateMachine<CPlayer>*>(dynamic_cast<CKnightPlayer*>(m_pPlayer)->m_pStateMachine.get());
						if (stateMachine->GetCurrentState() == Idle_Player::GetInst() ||
							stateMachine->GetCurrentState() == Run_Player::GetInst()) {
							if (dynamic_cast<CKnightPlayer*>(m_pPlayer)->m_nRemainPotions > 0) {
								if (((CPlayer*)m_pPlayer)->m_bIsDash)
								{
									((CPlayer*)m_pPlayer)->m_bIsDash = false;
									((CPlayer*)m_pPlayer)->SetSpeedUperS(((CPlayer*)m_pPlayer)->GetSpeedUperS() / 2.0f);
								}
								dynamic_cast<CKnightPlayer*>(m_pPlayer)->DrinkPotion();
								dynamic_cast<CKnightPlayer*>(m_pPlayer)->m_bIsDrinkPotion = true;
							}
						}
					}
				}
			}
			break;
		case 'z':
		case 'Z':
			if (m_pPlayer)
			{
				if (!((CPlayer*)m_pPlayer)->m_bCharged && dynamic_cast<CSquareBar*>(m_pUIObject[14].get())->IsFullGauge())
				{
					if (!(((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState() == Idle_Player::GetInst()) &&
						!(((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState() == Run_Player::GetInst()))
						break;
					dynamic_cast<CSquareBar*>(m_pUIObject[14].get())->Reset();
					((CPlayer*)m_pPlayer)->m_bCharged = true;
					m_bPlayCutScene = true;

					((CCinematicCamera*)(m_pCinematicPlayerCamera.get()))->SetFocusPoint(m_pPlayer->GetPosition());
					if (dynamic_cast<CDollyCamera*>(m_pCinematicPlayerCamera.get()))
						dynamic_cast<CDollyCamera*>(m_pCinematicPlayerCamera.get())->CaculateCubicPolyData();
					((CCinematicCamera*)(m_pCinematicPlayerCamera.get()))->PlayCinematicCamera();
					m_pCurrentCamera = m_pCinematicPlayerCamera.get();
					m_curSceneProcessType = SCENE_PROCESS_TYPE::CINEMATIC;
					/*m_pVertexPointParticleObject->SetTextureIndex(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
					m_pVertexPointParticleObject->SetFieldSpeed(0.0f);
					m_pVertexPointParticleObject->SetColor(XMFLOAT3(0.4745, 0.9254, 1.0));
					m_pVertexPointParticleObject->SetRotateFactor(true);
					m_pVertexPointParticleObject->SetSpeed(5.0f);
					m_pVertexPointParticleObject->SetSize(XMFLOAT2(0.1, 0.3));
					m_pVertexPointParticleObject->EmitParticle(0);
					m_pVertexPointParticleObject->SetEmit(true);*/
				}
			}
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_CONTROL:
			if (m_pPlayer)
			{
				if (((CPlayer*)m_pPlayer)->m_bIsDash)
				{
					((CPlayer*)m_pPlayer)->m_bIsDash = false;
					((CPlayer*)m_pPlayer)->SetSpeedUperS(((CPlayer*)m_pPlayer)->GetSpeedUperS() / 2.0f);
				}
			}
			break;
		case 'w':
		case 'W':
			if (wParam == 'w' || wParam == 'W') dwDirection &= (~DIR_FORWARD);
			break;
		case 's':
		case 'S':
			if (wParam == 's' || wParam == 'S') dwDirection &= (~DIR_BACKWARD);
			break;
		case 'a':
		case 'A':
			if (wParam == 'a' || wParam == 'A') dwDirection &= (~DIR_LEFT);
			break;
		case 'd':
		case 'D':
			if (wParam == 'd' || wParam == 'D') dwDirection &= (~DIR_RIGHT);
			break;
		case 'q':
		case 'Q':
			if (wParam == 'q' || wParam == 'Q')dwDirection &= (~DIR_DOWN);
			break;
		case 'e':
		case 'E':
			if (wParam == 'e' || wParam == 'E') dwDirection &= (~DIR_UP);
			break;
		}
	default:
		break;
	}

	if (dynamic_cast<CUIObject*>(m_pUIObject[13].get())->Reset())
	{
		MainSceneReset(hWnd);

		while (m_iCursorHideCount > 0)
		{
			m_iCursorHideCount--;
			ShowCursor(true);
		}
		ClipCursor(NULL);

		return SCENE_RETURN_TYPE::SWITCH_LOGOSCENE;
	}

	return SCENE_RETURN_TYPE::NONE;
}
void CMainTMPScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateGraphicsRootSignature(pd3dDevice);
	CreateComputeRootSignature(pd3dDevice);

	std::unique_ptr<RegisterArticulationListener> listener = std::make_unique<RegisterArticulationListener>();
	listener->SetScene(this);
	listener->SetEnable(true);
	m_pListeners.push_back(std::move(listener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::REQUEST_REGISTERARTI, m_pListeners.back().get(), nullptr);


	std::unique_ptr<RegisterArticulationSleepListener> slistener = std::make_unique<RegisterArticulationSleepListener>();
	slistener->SetScene(this);
	slistener->SetEnable(true);
	m_pListeners.push_back(std::move(slistener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::REQUEST_SLEEPARTI, m_pListeners.back().get(), nullptr);




	UINT ncbElementBytes = ((sizeof(DissolveParams) + 255) & ~255); //256의 배수
	m_pd3dcbDisolveParams = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbDisolveParams->Map(0, NULL, (void**)&m_pcbMappedDisolveParams);

	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	m_pFloatingCamera = std::make_unique<CFloatingCamera>();
	m_pFloatingCamera->Init(pd3dDevice, pd3dCommandList);
	m_pFloatingCamera->SetPosition(XMFLOAT3(0.0f, 400.0f, -100.0f));

	m_pMainSceneCamera = std::make_unique<CThirdPersonCamera>();
	m_pMainSceneCamera->Init(pd3dDevice, pd3dCommandList);

	std::unique_ptr<CCamera> FirstCinematicCamera = std::make_unique<CCamera>();
	FirstCinematicCamera->SetPosition(XMFLOAT3{ 129.561584, 15.859521, 146.650757 });
	FirstCinematicCamera->SetLookVector(XMFLOAT3{ -0.403303, -0.459168, -0.791525 });
	FirstCinematicCamera->SetUpVector(XMFLOAT3{ -0.208458, 0.888349, -0.409121 });
	FirstCinematicCamera->SetRightVector(XMFLOAT3{ -0.891006, -0.000000, 0.453991 });

	m_vCinematicCameraLocations.push_back(std::move(FirstCinematicCamera));

	m_pCinematicSceneCamera = std::make_unique<CCinematicCamera>();
	m_pCinematicSceneCamera->Init(pd3dDevice, pd3dCommandList);

	m_pCinematicPlayerCamera = std::make_unique<CDollyCamera>();
	m_pCinematicPlayerCamera->Init(pd3dDevice, pd3dCommandList);
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->SetFocusMode(CINEMATIC_FOCUSMODE::FOUCS_PLAYER);

	CCamera camera;
	camera.SetPosition(XMFLOAT3(0, 0, MeterToUnit(0.0f))); // dummy
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 1.9);
	camera.SetPosition(XMFLOAT3(0, 0, MeterToUnit(1.0f)));
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 0.95);
	camera.SetPosition(XMFLOAT3(MeterToUnit(1.1f), MeterToUnit(2.0f), 0));
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 0.95);
	camera.SetPosition(XMFLOAT3(0, MeterToUnit(0.5), MeterToUnit(1.8f)));
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 1.0);
	camera.SetPosition(XMFLOAT3(MeterToUnit(-2.5f), MeterToUnit(2.0f), MeterToUnit(1.8f)));
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 0.3);
	camera.SetPosition(XMFLOAT3(MeterToUnit(-3.5f), MeterToUnit(2.5f), 0));
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 0.2);
	XMFLOAT3 LastOffset = XMFLOAT3(0, 0.5, -1);
	LastOffset = Vector3::Normalize(LastOffset);
	LastOffset = Vector3::ScalarProduct(LastOffset, MeterToUnit(3.0f), false);
	camera.SetPosition(LastOffset);
	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->AddCameraInfo(&camera, 0.3);

	dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->SetSimulationDimension(CINEMATIC_SIMULATION_DIMENSION::DIMENSION_LOCAL);



#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
#endif

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthRenderShader = std::make_unique<CDepthRenderShader>();
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pDepthRenderShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT, 1);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_pDepthRenderShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0,
		((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture()->GetTextures() + ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetBakedDepthTexture()->GetTextures()
	);
	m_pDepthRenderShader->CreateShaderResourceViews(pd3dDevice, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetDepthTexture(), 0, 9);
	m_pDepthRenderShader->CreateShaderResourceViews(pd3dDevice, ((CDepthRenderShader*)m_pDepthRenderShader.get())->GetBakedDepthTexture(), 0, 9);

	CModelShader::GetInst()->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	CModelShader::GetInst()->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CModelShader::GetInst()->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1000);

	for (int i = 0; i < MAX_GOBLIN_NUM; ++i)
	{
		std::unique_ptr<CMonster> m_pMonsterObject = std::make_unique<CGoblinObject>(pd3dDevice, pd3dCommandList, 1);
		m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, 70));
		m_pMonsterObject->SetScale(2.0f, 2.0f, 2.0f);
		m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
		m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
		m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
		m_pMonsterObject->CreateArticulation(1.0f);
		CMonsterPool::GetInst()->SetNonActiveMonster(MONSTER_TYPE::GOBLIN, m_pMonsterObject.get());
		m_pEnemys.push_back(std::move(m_pMonsterObject));
	}

	for (int i = 0; i < MAX_ORC_NUM; ++i)
	{
		std::unique_ptr<CMonster> m_pMonsterObject = std::make_unique<COrcObject>(pd3dDevice, pd3dCommandList, 1);
		m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, 70));
		m_pMonsterObject->SetScale(2.0f, 2.0f, 2.0f);
		m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
		m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
		m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
		m_pMonsterObject->CreateArticulation(1.0f);
		CMonsterPool::GetInst()->SetNonActiveMonster(MONSTER_TYPE::ORC, m_pMonsterObject.get());
		m_pEnemys.push_back(std::move(m_pMonsterObject));
	}

	for (int i = 0; i < MAX_SKELETON_NUM; ++i)
	{
		std::unique_ptr<CMonster> m_pMonsterObject = std::make_unique<CSkeletonObject>(pd3dDevice, pd3dCommandList, 1);
		m_pMonsterObject->SetPosition(XMFLOAT3(190, 50, 70));
		m_pMonsterObject->SetScale(2.0f, 2.0f, 2.0f);
		m_pMonsterObject->Rotate(0.0f, 180.0f, 0.0f);
		m_pMonsterObject->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
		m_pMonsterObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(10.0f, 10.0f, 10.0f);
		m_pMonsterObject->CreateArticulation(1.0f);
		CMonsterPool::GetInst()->SetNonActiveMonster(MONSTER_TYPE::SKELETON, m_pMonsterObject.get());
		m_pEnemys.push_back(std::move(m_pMonsterObject));
	}




	// CollisionChecker 생성
	m_pCollisionChecker = std::make_unique<CollisionChecker>();

	// Light 생성
	m_pLight = std::make_unique<CLight>();
	m_pLight->CreateLightVariables(pd3dDevice, pd3dCommandList);


	m_pMap = std::make_unique<CMap>();
	m_pMap->Init(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
	//m_pMap->GetTerrain()->SetPosition(XMFLOAT3(86.4804, -46.8876 - 46.8876 * 0.38819 + 6.5f, -183.7856));
	m_pMap->GetTerrain()->SetRigidStatic();
	m_pMap->LoadSceneFromFile(pd3dDevice, pd3dCommandList, "Object/Scene/Scene.bin");

	m_pMainSceneCamera->SetUpdatedContext(m_pMap.get());

	std::shared_ptr<CTerrainDetailShader> pShader = std::make_shared<CTerrainDetailShader>();
	pShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	pShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 2);
	m_pDetailObject = std::make_unique<CDetailObject>(pd3dDevice, pd3dCommandList, pShader, m_pMap->GetTerrain().get());
	m_pDetailObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	int index = 0;

	m_IObjectIndexs.resize(m_pEnemys.size() + m_pMap->GetMapObjects().size());

	for (int i = 0; i < m_pEnemys.size(); ++i) {
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pEnemys[i].get());
		((CPhysicsObject*)m_pEnemys[i].get())->SetUpdatedContext(m_pMap.get());
		m_IObjectIndexs[i] = index++;

		m_pCollisionChecker->RegisterObject(m_pEnemys[i].get());
	}

	for (int i = 0; i < m_pMap->GetMapObjects().size(); ++i) {
		((CDepthRenderShader*)m_pDepthRenderShader.get())->RegisterObject(m_pMap->GetMapObjects()[i].get(), 0); // register type map
		m_IObjectIndexs[i] = index++;
	}

	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetLight(m_pLight->GetLights());
	((CDepthRenderShader*)m_pDepthRenderShader.get())->SetTerrain(m_pMap->GetTerrain().get());

	m_pSunLightShader = std::make_unique<CSunLightShader>();
	m_pSunLightShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	m_pPostProcessShader = std::make_shared<CPostProcessShader>();
	m_pPostProcessShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pPostProcessShader->BuildObjects(pd3dDevice, pd3dCommandList);

	std::shared_ptr<CTexture> pSkyBoxTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/Epic_BlueSunset.dds", RESOURCE_TEXTURE_CUBE, 0);

	m_pSkyBoxShader = std::make_unique<CSkyBoxShader>();
	m_pSkyBoxShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSkyBoxShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 1);
	m_pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture.get(), 0, 10);

	m_pSkyBoxObject = std::make_unique<CSkyBox>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), pSkyBoxTexture);

	m_pTextureManager = std::make_unique<CTextureManager>();
	m_pTextureManager->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 100);
	LoadTextureObject(pd3dDevice, pd3dCommandList);
	m_pTextureManager->CreateResourceView(pd3dDevice, 0);

	m_pParticleShader = std::make_unique<CParticleShader>();
	m_pParticleShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);

	m_pSlashHitShader = std::make_unique<CSlashHitShader>();
	m_pSlashHitShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteAttackObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::SmokeTexture, L"Image/SmokeImages/Smoke2.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
		m_pSpriteAttackObjects.push_back(std::move(m_pSpriteAttackObject));
	}

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pSpriteObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::BillBoardTexture, L"Image/BillBoardImages/Circle1.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), 5);
		m_pTerrainSpriteObject.push_back(std::move(m_pSpriteObject));
	}

	for (int i = 0; i < 1; ++i)
	{
		std::unique_ptr<CGameObject> m_pParticleObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/ParticleImages/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
		m_pParticleObjects.push_back(std::move(m_pParticleObject));
		//CPlayerParticleObject::GetInst()->SetTrailParticleObjects(m_pParticleObjects.back().get());
	}
	for (int i = 0; i < m_pEnemys.size(); ++i) {
		dynamic_cast<CMonster*>(m_pEnemys[i].get())->m_ParticleCompParam.pObject = m_pParticleObjects.front().get();
	}

	m_pSlashHitObjects = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::BillBoardTexture, L"Image/BillBoardImages/TextureFlash2.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pSlashHitShader.get(), RECOVERY_PARTICLE);
	m_pSmokeObject = std::make_unique<CSmokeParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::SmokeTexture, L"Image/SmokeImages/Smoke2.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SMOKE_PARTICLE);

	m_pVertexPointParticleObject = std::make_unique<CVertexPointParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/ParticleImages/RoundSoftParticle.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.5f, 0.5f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);
	//for (int i = 0; i < MAX_UPDOWN_PARTICLE_OBJECT; i++)
	//{
	//	std::unique_ptr<CParticleObject> m_pMeterorObject = std::make_unique<CParticleObject>(m_pTextureManager->LoadParticleTexture(L"ParticleImage/Meteor.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), RECOVERY_PARTICLE);
	//	m_pUpDownParticleObjects.push_back(std::move(m_pMeterorObject));
	//}


	m_pTrailParticleObjects = std::make_unique<CParticleObject>(m_pTextureManager->LoadTextureIndex(TextureType::ParticleTexture, L"Image/BillBoardImages/Effect3.dds"), pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(2.0f, 2.0f), MAX_PARTICLES, m_pParticleShader.get(), SPHERE_PARTICLE);

	CPlayerParticleObject::GetInst()->SetSmokeObjects(m_pSmokeObject.get());
	CPlayerParticleObject::GetInst()->SetTrailParticleObjects(m_pTrailParticleObjects.get());
	CPlayerParticleObject::GetInst()->SetVertexPointParticleObjects(m_pVertexPointParticleObject.get());

	m_pHDRComputeShader = std::make_unique<CHDRComputeShader>();
	m_pHDRComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());

	m_pBloomComputeShader = std::make_unique<CBloomShader>();
	m_pBloomComputeShader->CreateShader(pd3dDevice, pd3dCommandList, GetComputeRootSignature());
	m_pBloomComputeShader->CreateBloomUAVResource(pd3dDevice, pd3dCommandList, 1920, 1080);

	m_pLensFlareShader = std::make_unique<CLensFlareShader>();
	m_pLensFlareShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, 0);
	m_pLensFlareShader->BuildObjects(pd3dDevice, pd3dCommandList);

	m_pBreakScreenShader = std::make_unique<CBreakScreenEffectShader>();
	m_pBreakScreenShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pBreakScreenShader->BuildObjects(pd3dDevice, pd3dCommandList);
	m_pBreakScreenShader->SetCrackIndex(m_pTextureManager->GetTextureOffset(TextureType::UniformTexture));

	/*dynamic_cast<ImpactEffectComponent*>(ChargeAttack_Player::GetInst()->GetImpactComponent())->SetTextureIndex(m_pTextureManager->GetTextureOffset(TextureType::BillBoardTexture) + 3);
	dynamic_cast<ParticleComponent*>(ChargeAttack_Player::GetInst()->GetParticleComponent())->SetTextureIndex(m_pTextureManager->GetTextureOffset(TextureType::ParticleTexture));
	dynamic_cast<SlashHitComponent*>(ChargeAttack_Player::GetInst()->GetSlashHitComponent())->SetTextureIndex(m_pTextureManager->GetTextureOffset(TextureType::ParticleTexture));*/

	std::unique_ptr<SpecialMoveDamageListener> SPDlistener = std::make_unique<SpecialMoveDamageListener>();
	SPDlistener->SetEnable(true);
	SPDlistener->SetObjects(&m_pEnemys);
	SPDlistener->SetParticleObject(m_pParticleObjects.front().get());
	SPDlistener->SetImpactObject(m_pSpriteAttackObjects.front().get());
	SPDlistener->SetScene(this);
	m_pListeners.push_back(std::move(SPDlistener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::SPECIALMOVE_DAMAGED, m_pListeners.back().get(), nullptr);

	std::unique_ptr<SpecialMoveListener> splistener = std::make_unique<SpecialMoveListener>();
	splistener->SetShader(m_pBreakScreenShader.get());
	splistener->SetEnable(true);
	m_pListeners.push_back(std::move(splistener));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SPMOVE, m_pListeners.back().get(), nullptr);

	/*m_pSwordTrailShader = std::make_unique<CSwordTrailShader>();
	m_pSwordTrailShader->CreateGraphicsPipelineState(pd3dDevice, GetGraphicsRootSignature(), 0);
	m_pSwordTrailShader->BuildObjects(pd3dDevice, pd3dCommandList);*/

	for (int i = 0; i < 5; ++i) {
		std::unique_ptr<CSwordTrailObject> pSwordtrail = std::make_unique<CSwordTrailObject>(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pSwordTrailShader.get());
		m_pSwordTrailObjects.push_back(std::move(pSwordtrail));
	}

	m_pUIObjectShader = std::make_unique<CUIObjectShader>();
	m_pUIObjectShader->CreateShader(pd3dDevice, GetGraphicsRootSignature(), 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);

	// 플레이어 UI

	std::unique_ptr<CUIObject>  pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(512.f * 0.16f, 512.f * 0.16f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f - 80.f, FRAME_BUFFER_HEIGHT * 0.878f + 80.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/IconFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(512.f * 0.15f, 512.f * 0.15f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f - 80.f, FRAME_BUFFER_HEIGHT * 0.878f + 80.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/PlayerIcon.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	// Monster UI
	pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.443f, 64.f * 0.65f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.2f - 7.f, FRAME_BUFFER_HEIGHT * 0.878f + 70.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/MonsterHPFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.44f, 64.f * 0.6f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.2f - 7.f, FRAME_BUFFER_HEIGHT * 0.878f + 70.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/MonsterHP.dds")); // HP // 3
	m_pUIObject.push_back(std::move(pUIObject));

	// Monster UI
	pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.343f, 64.f * 0.6f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.17f, FRAME_BUFFER_HEIGHT * 0.86f + 70.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/MonsterHPFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));


	pUIObject = std::make_unique<CSTAMINAObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.34f, 64.f * 0.6f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.17f, FRAME_BUFFER_HEIGHT * 0.85995f + 70.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Stamina.dds")); // STAMINA 
	m_pUIObject.push_back(std::move(pUIObject));



	// Monster UI
	pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.8f, 64.f * 0.6f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.15f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/MonsterHPFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));


	pUIObject = std::make_unique<CMonsterHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.8f, 64.f * 0.6f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.15f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/MonsterHP.dds"));
	m_pUIObject.push_back(std::move(pUIObject));


	// Icon Frame
	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(251.f * 0.43f, 254.f * 0.48f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f - 100.f, FRAME_BUFFER_HEIGHT * 0.1f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/IconFrame.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	// HP Icon
	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(256.f * 0.4f, 256.f * 0.45f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f - 100., FRAME_BUFFER_HEIGHT * 0.1f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/HpIcon.dds"));
	m_pUIObject.push_back(std::move(pUIObject));

	//// Combo UI
	pUIObject = std::make_unique<CComboNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(64.f * 1.f, 60.f * 1.f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.88f, FRAME_BUFFER_HEIGHT * 0.5f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Texture2.dds")); // 0
	m_pUIObject.push_back(std::move(pUIObject));

	// ITEM_N UI

	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(64.f * 0.4f, 60.f * 0.4f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.065f, FRAME_BUFFER_HEIGHT * 0.06f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Texture2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	std::unique_ptr<PotionRemainUpdateComponent> pUpdateUINumListener = std::make_unique<PotionRemainUpdateComponent>();
	pUpdateUINumListener->SetEnable(true);
	(pUpdateUINumListener.get())->SetUIObject(pUIObject.get());
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::DRINK_POTION, pUpdateUINumListener.get(), nullptr);


	m_pListeners.push_back(std::move(pUpdateUINumListener));
	m_pUIObject.push_back(std::move(pUIObject));



	// Hits
	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(840.f * 0.15f, 360.f * 0.15f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.85f + 200.f, FRAME_BUFFER_HEIGHT * 0.5f - 15.f));
	pUIObject->SetTextureIndex(m_pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Hits.dds"));
	dynamic_cast<CUIObject*>(pUIObject.get())->SetColor(2.1f);
	m_pUIObject.push_back(std::move(pUIObject));


	

	pUIObject = std::make_unique<CResultFrame>(2, pd3dDevice, pd3dCommandList, 10.f, m_pTextureManager.get()); // 13
	pUIObject->SetEnable(false);
	m_pUIObject.push_back(std::move(pUIObject));

	//ChargeIcon
	pUIObject = std::make_unique<CSquareBar>(2, pd3dDevice, pd3dCommandList, 10.f, m_pTextureManager.get()); // 14
	m_pUIObject.push_back(std::move(pUIObject));


	m_StageInfoMap.emplace(std::pair<int, StageInfo>(0,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			3, 0,
			0, 0,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(1,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			0, 0,
			3, 0,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(2,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			0, 0,
			0, 0,
			3, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(3,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			0, 1,
			0, 0,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(4,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			5, 1,
			0, 0,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(5,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			2, 0,
			5, 1,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(6,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			4, 2,
			4, 2,
			0, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(7,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			7, 0,
			7, 0,
			7, 0,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
	m_StageInfoMap.emplace(std::pair<int, StageInfo>(8,
		StageInfo{
			std::vector<SpawnInfo>{SpawnInfo{
			8, 4,
			8, 4,
			8, 4,
			XMFLOAT3{ 113.664360f, 3.016271f, 123.066483f },
			12.5f}} }));
}
bool CMainTMPScene::ProcessInput(HWND hWnd, float fTimeElapsed)
{
	if (m_curSceneProcessType == SCENE_PROCESS_TYPE::CINEMATIC)
		return true;

	POINT ptCursorPos;
	float cxDelta, cyDelta;

	if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::FLOATING_MODE) {
		if (GetCapture() == hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
	}

	else if (Locator.GetMouseCursorMode() == MOUSE_CUROSR_MODE::THIRD_FERSON_MODE) {
		m_ptOldCursorPos = POINT(m_ScreendRect.left + (m_ScreendRect.right - m_ScreendRect.left) / 2,
			m_ScreendRect.top + (m_ScreendRect.bottom - m_ScreendRect.top) / 2
		);

		if (hWnd == GetFocus())
		{
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 8.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 8.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
	}

	m_pCurrentCamera->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed);
	((CPlayer*)m_pPlayer)->ProcessInput(dwDirection, cxDelta, cyDelta, fTimeElapsed, m_pCurrentCamera);

	return true;
}

struct ThreadData
{
	std::vector<std::unique_ptr<CGameObject>>* pEnemys;
	int offset;
	int range;
	float fTimeElapsed;
	DissolveParams* dissolveParams;
};

DWORD WINAPI AnimateEnemysProcess(LPVOID data) {
	ThreadData threadData;
	memcpy(&threadData, data, sizeof(ThreadData));

	std::vector<std::unique_ptr<CGameObject>>* pEnemys = threadData.pEnemys;
	int offset = threadData.offset;
	int range = threadData.range;
	float fTimeElapsed = threadData.fTimeElapsed;
	DissolveParams* dissolveParams = threadData.dissolveParams;

	for (int i = offset; i < offset + range; ++i) {
		(*pEnemys)[i]->Update(fTimeElapsed);
		dissolveParams->dissolveThreshold[i] = (*pEnemys)[i]->m_fDissolveThrethHold;
	}

	return 0;
};

void CMainTMPScene::UpdateObjects(float fTimeElapsed)
{
	if (m_curSceneProcessType == SCENE_PROCESS_TYPE::WAITING)
	{
		m_fWaitingTime += fTimeElapsed;
		if (m_fWaitingTime > MAX_WAIT_TIME)
		{
			m_pCurrentCamera = m_pMainSceneCamera.get();
			m_curSceneProcessType = SCENE_PROCESS_TYPE::NORMAL;
		}
	}

	if (m_pPlayer) {
		if (!dynamic_cast<CPlayer*>(m_pPlayer)->m_pSwordTrailReference)
			dynamic_cast<CPlayer*>(m_pPlayer)->m_pSwordTrailReference = m_pSwordTrailObjects.data();
		if (m_pBreakScreenShader->GetPlayer() == nullptr)
			m_pBreakScreenShader->SetPlayer(dynamic_cast<CPlayer*>(m_pPlayer));
	}

	AnimationCompParams animation_comp_params;
	animation_comp_params.pObjects = &m_pEnemys;
	animation_comp_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<AnimationCompParams>(MessageType::UPDATE_OBJECT, &animation_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

	m_pLight->Update((CPlayer*)m_pPlayer);

	static std::vector<HANDLE> ObjThreadHandles;
	if (ObjThreadHandles.size() == 0)
		ObjThreadHandles.resize(10);

	int nThreads = m_pEnemys.size() / 50;
	nThreads += 1;

	//
	/*int nRemainEnemys = m_pEnemys.size();
	int offset = 0;
	int MaxRange = 50;

	for (int i = 0; i < nThreads; ++i) {
		ThreadData data;
		data.fTimeElapsed = fTimeElapsed;
		data.pEnemys = &m_pEnemys;
		data.offset = offset;
		data.range = min(nRemainEnemys, MaxRange);
		data.dissolveParams = m_pcbMappedDisolveParams;

		HANDLE hGameThread = CreateThread(NULL, 0, AnimateEnemysProcess, (LPVOID)&data, 0, NULL);
		ObjThreadHandles[i] = hGameThread;

		offset += data.range;
		nRemainEnemys -= data.range;
	}

	WaitForMultipleObjects(nThreads, ObjThreadHandles.data(), true, INFINITE);
	for (int i = 0; i < nThreads; ++i) {
		CloseHandle(ObjThreadHandles[i]);
	}*/

	if (!m_bPlayCutScene) {
		for (int i = 0; i < m_pEnemys.size(); ++i) {
			m_pEnemys[i]->Update(fTimeElapsed);
			m_pcbMappedDisolveParams->dissolveThreshold[i] = m_pEnemys[i]->m_fDissolveThrethHold;
		}
	}



	m_pMap->Update(fTimeElapsed);


	m_pCollisionChecker->UpdateObjects(fTimeElapsed);


	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Update(fTimeElapsed);
	}

	m_pPlayer->Update(fTimeElapsed);

	static bool isSetVPObject = false;
	if (!isSetVPObject) {
		CGameObject* obj = m_pPlayer->FindFrame("Sword_low");
		m_pVertexPointParticleObject->SetWorldMatrixReference(&obj->m_xmf4x4World);
		// body
		m_pVertexPointParticleObject->SetVertexPointsFromSkinnedSubeMeshToRandom(m_pPlayer->m_pSkinnedAnimationController->m_ppSkinnedMeshes[0], 6, m_pPlayer->m_pSkinnedAnimationController.get());
		//m_pVertexPointParticleObject->SetVertexPointsFromSkinnedMeshToRandom(m_pPlayer->m_pSkinnedAnimationController->m_ppSkinnedMeshes[0], m_pPlayer->m_pSkinnedAnimationController.get());
		//m_pVertexPointParticleObject->SetVertexPointsFromStaticMeshToUniform(obj->m_pMesh.get());
		//m_pVertexPointParticleObject->SetVertexPointsFromStaticMeshToRandom(obj->m_pMesh.get());
		isSetVPObject = true;
		CDrinkPotionCallbackHandler* handler = dynamic_cast<CDrinkPotionCallbackHandler*>((dynamic_cast<CKnightPlayer*>(m_pPlayer)->m_pAnimationcHandlers[0].get()));
		handler->m_pVertexPointParticleObject = m_pVertexPointParticleObject.get();
	}

	UIUpdate((CPlayer*)m_pPlayer);

	m_pBreakScreenShader->AnimateObjects(fTimeElapsed);


	// Update Camera

	XMFLOAT3 xmf3PlayerPos = XMFLOAT3{
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
		 m_pPlayer->GetPosition().y,
		((CKnightPlayer*)m_pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43 };
	xmf3PlayerPos.y += MeterToUnit(0.9f);

	m_pCurrentCamera->Update(xmf3PlayerPos, fTimeElapsed);

	CameraUpdateParams camera_update_params;
	camera_update_params.pCamera = m_pMainSceneCamera.get();
	camera_update_params.pPlayer = m_pPlayer;
	camera_update_params.fElapsedTime = fTimeElapsed;
	CMessageDispatcher::GetInst()->Dispatch_Message<CameraUpdateParams>(MessageType::UPDATE_CAMERA, &camera_update_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

	PlayerParams player_params;
	player_params.pPlayer = m_pPlayer;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER, &player_params, nullptr);
}

#define WITH_LAG_DOLL_SIMULATION

void CMainTMPScene::Update(float fTimeElapsed)
{
#ifdef WITH_LAG_DOLL_SIMULATION
	static float SimulateElapsedTime = 0.0f;

	SimulateElapsedTime += fTimeElapsed;
	if (!b_simulation) {
		Locator.GetPxScene()->simulate(SimulateElapsedTime);
		SimulateElapsedTime = 0.0f;
		b_simulation = true;
	}
	if (Locator.GetPxScene()->fetchResults(false)) {
		SleepArticulations();
		RegisterArticulations();
		UpdateObjectArticulation();
		b_simulation = false;
	}



#endif // WITH_LAG_DOLL_SIMULATION
	UpdateObjects(fTimeElapsed);
	m_pLensFlareShader->UpdateFlareAlpha(fTimeElapsed);
}
void CMainTMPScene::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	m_pPostProcessShader->OnPrepareRenderTarget(pd3dCommandList, 1, pd3dRtvCPUHandles, d3dDepthStencilBufferDSVCPUHandle);
}
void CMainTMPScene::Enter(HWND hWnd)
{
	if (m_fGameStartTime == -FLT_MAX)
	{
		m_fGameStartTime = Locator.GetTimer()->GetTotalTime();

		// 씨네마틱 카메라로 게임을 시작하게 설정
	
		((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->AddPlayerCameraInfo((CPlayer*)m_pPlayer, m_pMainSceneCamera.get());
		((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->AddCameraInfo(m_vCinematicCameraLocations[0].get());

		if (dynamic_cast<CDollyCamera*>(m_pCinematicSceneCamera.get())) {
			dynamic_cast<CDollyCamera*>(m_pCinematicSceneCamera.get())->CaculateCubicPolyData();
		}

		((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->PlayCinematicCamera();

		// 시작은 씨네마틱 카메라
		m_pCurrentCamera = m_pCinematicSceneCamera.get();
		m_curSceneProcessType = SCENE_PROCESS_TYPE::CINEMATIC;

		dynamic_cast<CCinematicCamera*>(m_pCinematicPlayerCamera.get())->SetLocalObject(m_pPlayer);

		if (m_pTrailParticleObjects)
			CPlayerParticleObject::GetInst()->SetTrailParticleObjects(m_pTrailParticleObjects.get());
	}
	
	Locator.SetMouseCursorMode(MOUSE_CUROSR_MODE::THIRD_FERSON_MODE);
	m_CurrentMouseCursorMode = MOUSE_CUROSR_MODE::THIRD_FERSON_MODE;

	if (m_iCursorHideCount < 1) {
		RECT screenRect;
		GetWindowRect(hWnd, &screenRect);
		m_ScreendRect = screenRect;
		SetCursorPos(screenRect.left + (screenRect.right - screenRect.left) / 2,
			screenRect.top + (screenRect.bottom - screenRect.top) / 2);

		m_iCursorHideCount++;
		ShowCursor(false);
		ClipCursor(&screenRect);
	}

	CSoundManager::GetInst()->PlaySound(m_strMainAmbientSoundPath, 1.0f, 0.0f);
	CSoundManager::GetInst()->PlaySound(m_strMainActionSoundPath, 1.0f, 0.0f);

	PostMessage(hWnd, WM_ACTIVATE, 0, 0);
}
void CMainTMPScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, float fCurrentTime, CCamera* pCamera)
{
	m_fCurrentTime = fCurrentTime;

	m_pCurrentCamera->OnPrepareRender(pd3dCommandList);

	m_pLight->Render(pd3dCommandList);

	m_pSunLightShader->Render(pd3dCommandList, m_pCurrentCamera);

	m_pSkyBoxShader->Render(pd3dCommandList, 0);
	m_pSkyBoxObject->Render(pd3dCommandList, m_pCurrentCamera);


	m_pTextureManager->SetTextureDescriptorHeap(pd3dCommandList);
	m_pTextureManager->UpdateShaderVariables(pd3dCommandList);
	//m_pTextureManager->UpdateShaderVariables(pd3dCommandList, TextureType::UniformTexture);

	bool bRimLightEnable = 0;
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &bRimLightEnable, 34);

	m_pMap->RenderTerrain(pd3dCommandList);

	for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->Update(fTimeElapsed);
		((CParticleObject*)(m_pTerrainSpriteObject[i].get()))->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}


	//((CParticleObject*)m_pSmokeObject.get())->Update(fTimeElapsed);
	//((CParticleObject*)m_pSmokeObject.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//((CParticleObject*)m_pSmokeObject.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());



	//for (int i = 0; i < m_pUpDownParticleObjects.size(); ++i)
	//{
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Update(fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	//	((CParticleObject*)m_pUpDownParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	//}

	/*((CParticleObject*)m_pTrailParticleObjects.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	*/
	//m_pBillBoardObjectShader->Render(pd3dCommandList, 0);



	CModelShader::GetInst()->Render(pd3dCommandList, 0);
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbDisolveParams->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);

	if (m_pPlayer)
	{
		m_pPlayer->Render(pd3dCommandList, true);
	}

	m_pMap->RenderMapObjects(pd3dCommandList);

	


	static float trailUpdateT = 0.0f;
	trailUpdateT += fTimeElapsed;
	if (trailUpdateT > 0.008f) {
		for (int i = 0; i < m_pSwordTrailObjects.size(); ++i) {
			CSwordTrailObject* trailObj = dynamic_cast<CSwordTrailObject*>(m_pSwordTrailObjects[i].get());
			switch (trailObj->m_eTrailUpdateMethod)
			{
			case TRAIL_UPDATE_METHOD::UPDATE_NEW_CONTROL_POINT:
				trailObj->SetNextControllPoint(&((CPlayer*)m_pPlayer)->GetTrailControllPoint(0), &((CPlayer*)m_pPlayer)->GetTrailControllPoint(1));
				break;
			case TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT:
				break;
			case TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT:
				trailObj->SetNextControllPoint(nullptr, nullptr);

				break;
			default:
				break;
			}
		}
		trailUpdateT = 0.0f;

		static int count = 100;
		count--;
		if (count < 1) {

			count = 100;
		}

	}

	/*m_pVertexPointParticleObject->SetTextureIndex(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
	m_pVertexPointParticleObject->SetFieldSpeed(0.0f);
	m_pVertexPointParticleObject->EmitParticle(5);
	m_pVertexPointParticleObject->SetEmit(true);*/

	/*std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::chrono::duration<double> result = end - start;
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("enemy update time :%f\n"), result.count());
	OutputDebugString(pstrDebug);*/

	UINT index = 0;
	for (int i = 0; i < m_pEnemys.size(); ++i)
	{
		pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &i, 33);
		m_pEnemys[i]->Render(pd3dCommandList, true);
	}

	//m_pDetailObject->Render(pd3dCommandList, false);
	/*m_pTextureManager->SetTextureDescriptorHeap(pd3dCommandList);
	m_pTextureManager->UpdateShaderVariables(pd3dCommandList);*/

#ifdef RENDER_BOUNDING_BOX
	CBoundingBoxShader::GetInst()->Render(pd3dCommandList, 0);
#endif

#define PostProcessing

#ifdef PostProcessing
	m_pPostProcessShader->Render(pd3dCommandList, pCamera);
#endif // PostProcessing
	m_pDetailObject->Render(pd3dCommandList, true);

	for (std::unique_ptr<CGameObject>& obj : m_pSwordTrailObjects) {
		obj->Render(pd3dCommandList, true);
	}


	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Animate(fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pParticleObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}

	for (int i = 0; i < m_pSpriteAttackObjects.size(); ++i)
	{
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Update(fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Animate(fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}


	((CParticleObject*)m_pTrailParticleObjects.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pTrailParticleObjects.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	((CParticleObject*)m_pSmokeObject.get())->Update(fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	((CParticleObject*)m_pSmokeObject.get())->Render(pd3dCommandList, nullptr, m_pParticleShader.get());

	{
		m_pVertexPointParticleObject->Update(fTimeElapsed);
		m_pVertexPointParticleObject->Animate(fTimeElapsed);
		m_pVertexPointParticleObject->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
		m_pVertexPointParticleObject->Render(pd3dCommandList, nullptr, m_pParticleShader.get());
	}


	m_pSlashHitObjects->Update(fTimeElapsed);
	m_pSlashHitObjects->Animate(fTimeElapsed);
	m_pSlashHitObjects->UpdateShaderVariables(pd3dCommandList, fCurrentTime, fTimeElapsed);
	m_pSlashHitObjects->Render(pd3dCommandList, nullptr, m_pSlashHitShader.get());

	/*m_pSwordTrailShader->Render(pd3dCommandList, pCamera, 0);*/

	m_pLensFlareShader->CalculateFlaresPlace(m_pCurrentCamera, &m_pLight->GetLights()[0]);
	m_pLensFlareShader->Render(pd3dCommandList, 0);

	ID3D12Resource* pd3dSource;
	ID3D12Resource* pd3dDestination;

	m_pBreakScreenShader->Render(pd3dCommandList, 0);

	if (m_pBreakScreenShader->GetEnable()) {
		pd3dDestination = m_pPostProcessShader->GetTextureResource(0);
		pd3dSource = m_pPostProcessShader->GetTextureResource(4);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		pd3dCommandList->CopyResource(pd3dDestination, pd3dSource);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	}

	if (m_pd3dComputeRootSignature) pd3dCommandList->SetComputeRootSignature(m_pd3dComputeRootSignature.Get());


	{
		m_pBloomComputeShader->Dispatch(pd3dCommandList);

		pd3dSource = m_pBloomComputeShader->m_pBloomedTexture->GetResource(0);
		pd3dDestination = m_pHDRComputeShader->m_pSourceTexture->GetResource(0);

		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		pd3dCommandList->CopyResource(pd3dDestination, pd3dSource);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	}

	m_pUIObjectShader->Render(pd3dCommandList, 0);
	for (int i = 0; i < m_pUIObject.size(); i++)
	{
		m_pUIObject[i]->Update(fTimeElapsed);
		m_pUIObject[i]->Render(pd3dCommandList, false, nullptr);
	}

	m_pHDRComputeShader->Dispatch(pd3dCommandList);

	pd3dSource = m_pHDRComputeShader->m_pTextures->GetResource(0);
	pd3dDestination = m_pHDRComputeShader->m_pRenderTargetResource;

	::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	pd3dCommandList->CopyResource(pd3dDestination, pd3dSource);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dDestination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
	::SynchronizeResourceTransition(pd3dCommandList, pd3dSource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


}

void CMainTMPScene::OnPostRender()
{
	m_pCurrentCamera->OnPostRender();

	for (int i = 0; i < m_pParticleObjects.size(); ++i)
	{
		((CParticleObject*)m_pParticleObjects[i].get())->OnPostRender();
	}

	((CParticleObject*)m_pSmokeObject.get())->OnPostRender();

	for (int i = 0; i < m_pSpriteAttackObjects.size(); ++i)
	{
		((CParticleObject*)m_pSpriteAttackObjects[i].get())->OnPostRender();
	}
	for (int i = 0; i < m_pTerrainSpriteObject.size(); ++i)
	{
		((CParticleObject*)m_pTerrainSpriteObject[i].get())->OnPostRender();
	}
	m_pVertexPointParticleObject->OnPostRender();
	((CParticleObject*)m_pTrailParticleObjects.get())->OnPostRender();
	((CParticleObject*)m_pSlashHitObjects.get())->OnPostRender();

}
#include <functional>
#include <numeric>
void CMainTMPScene::UIUpdate(CPlayer* pPlayer)
{
	dynamic_cast<CBarObject*>(m_pUIObject[3].get())->Set_Value(((CPhysicsObject*)m_pPlayer)->m_fHP, ((CPhysicsObject*)m_pPlayer)->m_fTotalHP);
	dynamic_cast<CBarObject*>(m_pUIObject[5].get())->Set_Value(((CPlayer*)m_pPlayer)->m_fStamina, ((CPlayer*)m_pPlayer)->m_fTotalStamina);
	dynamic_cast<CBarObject*>(m_pUIObject[14].get())->Set_Value(((CPlayer*)m_pPlayer)->m_fSkillGauge, 100.f);
	dynamic_cast<CNumberObject*>(m_pUIObject[10].get())->UpdateNumber(((CPlayer*)m_pPlayer)->m_iCombo);
	
	dynamic_cast<CNumberObject*>(m_pUIObject[10].get())->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.88f - 40.f * (dynamic_cast<CNumberObject*>(m_pUIObject[10].get())->GetNumSize() - 1), FRAME_BUFFER_HEIGHT * 0.5f));
	OutputDebugString(std::to_wstring(((CPlayer*)m_pPlayer)->m_fStamina).c_str());
	OutputDebugString(L"\n");

	// ((CPlayer*)m_pPlayer)->m_iCombo / 10

	if (((CKnightPlayer*)(m_pPlayer))->GetMonsterAttack())
	{
		float CurrentHp = 0.f;
		StageInfo stageInfo = m_StageInfoMap.find(m_iStageNum - 1)->second;

		for (int i = 0; i < m_pEnemys.size(); i++)
			if (m_pEnemys[i]->m_bEnable) {
				CurrentHp += max(0.f, dynamic_cast<CMonster*>(m_pEnemys[i].get())->m_fHP);
			}

		dynamic_cast<CBarObject*>(m_pUIObject[7].get())->Set_Value(CurrentHp, stageInfo.m_fTotalHP);

		((CKnightPlayer*)(m_pPlayer))->SetMonsterAttack(false);

		if (CurrentHp == 0.0f)
		{
			m_pUIObject[6]->m_bEnable = false;
			m_pUIObject[7]->m_bEnable = false;
		}
	}

}

void CMainTMPScene::LoadTextureObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pTextureManager->LoadSphereBuffer(pd3dDevice, pd3dCommandList);
	m_pTextureManager->LoadTexture(TextureType::SmokeTexture, pd3dDevice, pd3dCommandList, L"Image/SmokeImages/Smoke2.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Explode_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Fire_Effect.dds", 5, 6);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Circle1.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Impact01_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Impact02_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Impact03_8x8.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Effect_02_8x6-48.dds", 8, 6);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Effect_05_8x8-64.dds", 8, 8);
	m_pTextureManager->LoadTexture(TextureType::BillBoardTexture, pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Effect_06_8x6-48.dds", 8, 6);

	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/TextureFlash2.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Effect0.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/T_Impact_33.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Bubble.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Effect_03_8x4-32.dds", 8, 4);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Fireflies.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Glow.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Glow_Ice_02.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Glow_Ice_05.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/Snowflake.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::ParticleTexture, pd3dDevice, pd3dCommandList, L"Image/ParticleImages/smoke.dds", 2, 2);

	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_11.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_21.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_20.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailBaseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/T_Sword_Slash_28.dds", 1, 1);

	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_4.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_14.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_8.dds", 1, 1);
	m_pTextureManager->LoadTexture(TextureType::TrailNoiseTexture, pd3dDevice, pd3dCommandList, L"Image/TrailImages/VAP1_Noise_17.dds", 1, 1);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Number/Numbers.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Number/Numbers2.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/MonsterHPFrame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/MonsterHP.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Stamina.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/HpIcon.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/IconFrame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/PlayerIcon.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Texture2.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Hits.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ResultFrame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ResultBackGround.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ResultScoreMenu.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ChargeIcon0.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/ChargeIcon1.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/SkillGauge.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Gradation.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/SkillFrame.dds", 0, 0);
	m_pTextureManager->LoadTexture(TextureType::UITexture, pd3dDevice, pd3dCommandList, L"Image/UiImages/Press.dds", 0, 0);

	m_pTextureManager->LoadTexture(TextureType::UniformTexture, pd3dDevice, pd3dCommandList, L"Image/UnifromImages/Cracks 3.dds", 0, 0);

	
}

void CMainTMPScene::HandleCollision(const CollideParams& params)
{
	std::vector<std::unique_ptr<CGameObject>>::iterator it = std::find_if(m_pParticleObjects.begin(), m_pParticleObjects.end(), [](const std::unique_ptr<CGameObject>& pParticleObject) {
		if (((CParticleObject*)pParticleObject.get())->CheckCapacity())
			return true;
		return false;
		});

	if (it != m_pParticleObjects.end())
	{
		ParticleCompParams particle_comp_params;
		particle_comp_params.pObject = (*it).get();
		particle_comp_params.xmf3Position = params.xmf3CollidePosition;
		dynamic_cast<CParticleObject*>((*it).get())->SetEmitAxis(((CPlayer*)m_pPlayer)->m_xmf3AtkDirection);
		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_PARTICLE, &particle_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());

		particle_comp_params.pObject = m_pSlashHitObjects.get();
		particle_comp_params.pObject = (*it).get();
		dynamic_cast<CParticleObject*>((*it).get())->SetEmitAxis(((CPlayer*)m_pPlayer)->m_xmf3AtkDirection);
		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_SLASHHITPARTICLE, &particle_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
	}

	it = std::find_if(m_pSpriteAttackObjects.begin(), m_pSpriteAttackObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
		if (((CParticleObject*)pBillBoardObject.get())->CheckCapacity())
			return true;
		return false;
		});


	if (it != m_pSpriteAttackObjects.end())
	{
		ImpactCompParams impact_comp_params;
		impact_comp_params.pObject = (*it).get();
		impact_comp_params.xmf3Position = params.xmf3CollidePosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<ImpactCompParams>(MessageType::UPDATE_BILLBOARD, &impact_comp_params, ((CPlayer*)m_pPlayer)->m_pStateMachine->GetCurrentState());
		CLogger::GetInst()->Log(std::string("Display Impact!!!!!!!!!!!!!"));
	}

	// Demo 사용x
	//it = std::find_if(m_pUpDownParticleObjects.begin(), m_pUpDownParticleObjects.end(), [](const std::unique_ptr<CGameObject>& pBillBoardObject) {
	//	if (((CParticleObject*)pBillBoardObject.get())->CheckCapacity())
	//		return true;
	//	return false;
	//	});

	//if (it != m_pUpDownParticleObjects.end())
	//{
	//	ParticleUpDownParams particleUpdown_comp_params;
	//	particleUpdown_comp_params.pObject = (*it).get();
	//	particleUpdown_comp_params.xmf3Position = params.xmf3CollidePosition;
	//	CMessageDispatcher::GetInst()->Dispatch_Message<ParticleUpDownParams>(MessageType::UPDATE_UPDOWNPARTICLE, &particleUpdown_comp_params, Damaged_Monster::GetInst());
	//}


}

void CMainTMPScene::HandleOnGround(const OnGroundParams& params)
{
	std::vector<std::unique_ptr<CGameObject>>::iterator TerrainSpriteit = std::find_if(m_pTerrainSpriteObject.begin(), m_pTerrainSpriteObject.end(), [](const std::unique_ptr<CGameObject>& pTerrainSpriteObject) {
		if (((CParticleObject*)pTerrainSpriteObject.get())->CheckCapacity())
			return true;
		return false;
		});

	if (TerrainSpriteit != m_pTerrainSpriteObject.end())
	{
		TerrainSpriteCompParams AttackSprite_comp_params;
		AttackSprite_comp_params.pObject = (*TerrainSpriteit).get();
		AttackSprite_comp_params.xmf3Position = params.xmf3OnGroundPosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<TerrainSpriteCompParams>(MessageType::UPDATE_SPRITE, &AttackSprite_comp_params, Spawn_Monster::GetInst());
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("바닥 충격 출력 %f, %f, %f\n"), params.xmf3OnGroundPosition.x, params.xmf3OnGroundPosition.y, params.xmf3OnGroundPosition.z);
		OutputDebugString(pstrDebug);
	}
	else
	{
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("No바닥 충격 출력 %f, %f, %f\n"), params.xmf3OnGroundPosition.x, params.xmf3OnGroundPosition.y, params.xmf3OnGroundPosition.z);
		OutputDebugString(pstrDebug);
	}
}

void CMainTMPScene::MainSceneReset(HWND hWnd)
{
	m_fGameStartTime = -FLT_MAX;
	dwDirection = 0;
	
	UIReset();

	((CPlayer*)m_pPlayer)->Reset();
	((CCinematicCamera*)(m_pCinematicSceneCamera.get()))->ClearCameraInfo();
	m_pMainSceneCamera->Reset();

	m_iStageNum = 0;
	for (int i = 0; i < m_pEnemys.size(); i++)
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pEnemys[i].get());
		CMonsterPool::GetInst()->SetNonActiveMonster(pMonster->GetMonsterType(), pMonster);
	}
	m_iTotalMonsterNum = 0;
}

void CMainTMPScene::UIReset()
{
	dynamic_cast<CUIObject*>(m_pUIObject[14].get())->Reset();

	m_pUIObject[6]->m_bEnable = false;
	m_pUIObject[7]->m_bEnable = false;
}

void CMainTMPScene::AllBackgroundSoundStop()
{
	CSoundManager::GetInst()->StopSound(m_strMainAmbientSoundPath);
	CSoundManager::GetInst()->StopSound(m_strMainActionSoundPath);
}
