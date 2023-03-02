#include "stdafx.h"
#include "Locator.h"
#include "Global.h"
#include "Component.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"
#include "..\Object\BillBoardComponent.h"
#include "..\Object\ParticleComponent.h"

CLocator::~CLocator()
{
	m_pEntityManager->Clear();
	m_pEntityManager.reset();
}

bool CLocator::Init()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	m_pPxPvd = physx::PxCreatePvd(*m_pFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
	m_pPxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	bool recordMemoryAllocations = true;

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(1.0f, 10.0f), recordMemoryAllocations, m_pPxPvd);

	physx::PxCudaContextManagerDesc cudaContextManagerDesc;

	m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());

	physx::PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(8); // 이 세가지 파라미터가 반드시 필요함.
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; // 각각 어떤 역할을 하는 지는 추가적으로 조사해볼 필요가 있음.
	SceneDesc.cudaContextManager = m_pCudaContextManager;

	SceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	SceneDesc.flags |= physx::PxSceneFlag::eSUPPRESS_READBACK;
	SceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;

	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	//SceneDesc.gravity = physx::PxVec3(0.0f, -981.0f, 0.0f);
	//SceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);

	m_pPxScene = m_pPhysics->createScene(SceneDesc);

	//physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));

	//physx::PxMaterial* material = m_pPhysics->createMaterial(0.5, 0.5, 0.5);
	//physx::PxShape* shape = m_pPhysics->createShape(physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material);

	//physx::PxRigidDynamic* actor = physx::PxCreateDynamic(*m_pPhysics, transform, physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material, 1.0f);


	//physx::PxRigidStatic* plane = physx::PxCreateStatic(*m_pPhysics, physx::PxTransform(physx::PxVec3(0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f))), physx::PxPlaneGeometry(), *material);

	////physx::PxActor* actor = m_pPhysics->createRigidDynamic(transform);

	////actor->attachShape(*actor);
	//m_pPxScene->addActor(*plane);
	//m_pPxScene->addActor(*actor);

	pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}


	m_pMessageDispatcher = std::make_unique<CMessageDispatcher>();
	m_pEntityManager = std::make_unique<CEntityManager>();

	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);

	CSoundManager::GetInst()->RegisterSound("Sound/David Bowie - Starman.mp3", false, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Air Cut by Langerium Id-84616.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Bloody Blade 2 by Kreastricon62 Id-323526.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/Swing by XxChr0nosxX Id-268227.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword by hello_flowers Id-37596.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword4 by Streety Id-30246.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword7 by Streety Id-30248.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh1.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh2.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh3.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh4.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh5.wav", false, SOUND_CATEGORY::SOUND_SHOCK);

	for (auto& [num, componentSet] : m_sComponentSets) {
		std::shared_ptr<CComponent> component;

		component = std::make_shared<CCameraMover>();
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraShaker>();
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraZoomer>();
		componentSet->AddComponent(component);

		component = std::make_shared<CDamageSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);
		component = std::make_shared<CEffectSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);
		component = std::make_shared<CShootSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);
		
		component = CDamageAnimationComponent::GetInst();
		componentSet->AddComponent(component);
		component = CShakeAnimationComponent::GetInst();
		componentSet->AddComponent(component);
		component = CStunAnimationComponent::GetInst();
		componentSet->AddComponent(component);

		component = std::make_shared<CAttackSpriteComponent>();
		componentSet->AddComponent(component);
		component = std::make_shared<CParticleComponent>();
		componentSet->AddComponent(component);
	}

	std::shared_ptr<CState<CPlayer>> state = std::make_shared<Idle_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk1_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk2_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk3_Player>();
	SetPlayerState(state);
	state = std::make_shared<Run_Player>();
	SetPlayerState(state);

	m_pMessageDispatcher = std::make_unique<CMessageDispatcher>();
	m_pEntityManager = std::make_unique<CEntityManager>();
	m_pSoundPlayer = std::make_unique<CSoundPlayer>();

	return true;
}

void CLocator::CreateSimulatorCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pSimulaterCamera = std::make_unique<CSimulatorCamera>();
	m_pSimulaterCamera->Init(pd3dDevice, pd3dCommandList);
	m_pSimulaterCamera->SetPosition(XMFLOAT3(-18.5f, 37.5f, -18.5f));
	m_pSimulaterCamera->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pSimulaterCamera->RegenerateViewMatrix();
}

void CLocator::CreateMainSceneCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pMainSceneCamera = std::make_unique<CThirdPersonCamera>();
	m_pMainSceneCamera->Init(pd3dDevice, pd3dCommandList);
}

CState<CPlayer>* CLocator::GetPlayerState(const std::type_info& type)
{
	/*auto comp = [](size_t hashcode, CState<CPlayer>& state) {
		return hashcode < typeid(state).hash_code();
	};*/
	//auto it = m_sPlayerStateSet.find<size_t, Statecomp, Statecomp::is_transparent>(type.hash_code());
	statedummy.first = type.hash_code();
	std::set<PlayerStatePair, Comp_PlayerState>::iterator it = m_sPlayerStateSet.find(statedummy);
	if (it == m_sPlayerStateSet.end())
		return nullptr;

	return it->second.get();
}

void CLocator::SetPlayerState(std::shared_ptr<CState<CPlayer>>& state)
{
	PlayerStatePair pair = std::make_pair(typeid(*state.get()).hash_code(), state);
	m_sPlayerStateSet.insert(pair);
}

void DataLoader::SaveComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets)
{
	std::string path;
	FILE* pInFile;

	for (auto [num, componentset] : ComponentSets) {
		path = file_path + std::to_string(num) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "wb");

		SaveComponentSet(pInFile, componentset.get());

		fclose(pInFile);
	}
}

void DataLoader::LoadComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets)
{
	std::string path;
	FILE* pInFile;

	for (auto [num, componentset] : ComponentSets) {
		path = file_path + std::to_string(num) + file_ext;

		::fopen_s(&pInFile, path.c_str(), "rb");
		if (!pInFile)
			continue;

		LoadComponentSet(pInFile, componentset.get());

		fclose(pInFile);
	}
}

void DataLoader::SaveComponentSet(FILE* pInFile, CComponentSet* componentset)
{
	std::string str = "<Components>:";
	WriteStringFromFile(pInFile, str);

	WriteStringFromFile(pInFile, std::string("<CCameraMover>:"));
	CCameraMover* mover = (CCameraMover*)componentset->FindComponent(typeid(CCameraMover));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, mover->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, mover->m_fMaxDistance);
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, mover->m_fMovingTime);
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, mover->m_fRollBackTime);
	WriteStringFromFile(pInFile, std::string("</CCameraMover>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraShaker>:"));
	CCameraShaker* shaker = (CCameraShaker*)componentset->FindComponent(typeid(CCameraShaker));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, shaker->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Duration>:"));
	WriteFloatFromFile(pInFile, shaker->m_fDuration);
	WriteStringFromFile(pInFile, std::string("<Magnitude>:"));
	WriteFloatFromFile(pInFile, shaker->m_fMagnitude);
	WriteStringFromFile(pInFile, std::string("</CCameraShaker>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraZoomer>:"));
	CCameraZoomer* zoomer = (CCameraZoomer*)componentset->FindComponent(typeid(CCameraZoomer));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, zoomer->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fMaxDistance);
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fMovingTime);
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fRollBackTime);
	WriteStringFromFile(pInFile, std::string("<IsZoomIN>:"));
	WriteIntegerFromFile(pInFile, zoomer->m_bIsIN);
	WriteStringFromFile(pInFile, std::string("</CCameraZoomer>:"));

	WriteStringFromFile(pInFile, std::string("<CEffectSoundComponent>:"));
	CEffectSoundComponent* effectsound = (CEffectSoundComponent*)componentset->FindComponent(typeid(CEffectSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, effectsound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, effectsound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CEffectSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CShootSoundComponent>:"));
	CShootSoundComponent* shootsound = (CShootSoundComponent*)componentset->FindComponent(typeid(CShootSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, shootsound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, shootsound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CShootSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CDamageSoundComponent>:"));
	CDamageSoundComponent* Damagesound = (CDamageSoundComponent*)componentset->FindComponent(typeid(CDamageSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, Damagesound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, Damagesound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, Damagesound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, Damagesound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CDamageSoundComponent>:"));

	str = "</Components>:";
	WriteStringFromFile(pInFile, str);
}

void DataLoader::LoadComponentSet(FILE* pInFile, CComponentSet* componentset)
{
	char buf[256];
	std::string str;
	str.resize(256);
	ReadStringFromFile(pInFile, buf);

	for (; ; )
	{
		ReadStringFromFile(pInFile, buf);

		if (!strcmp(buf, "<CCameraMover>:"))
		{
			CCameraMover* component = (CCameraMover*)componentset->FindComponent(typeid(CCameraMover));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					component->m_fMaxDistance = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					component->m_fMovingTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					component->m_fRollBackTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraMover>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraShaker>:"))
		{
			CCameraShaker* component = (CCameraShaker*)componentset->FindComponent(typeid(CCameraShaker));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Duration>:"))
				{
					component->m_fDuration = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Magnitude>:"))
				{
					component->m_fMagnitude = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraShaker>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraZoomer>:"))
		{
			CCameraZoomer* component = (CCameraZoomer*)componentset->FindComponent(typeid(CCameraZoomer));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					component->m_fMaxDistance = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					component->m_fMovingTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					component->m_fRollBackTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<IsZoomIN>:"))
				{
					component->m_bIsIN = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraZoomer>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CEffectSoundComponent>:"))
		{
			CEffectSoundComponent* component = (CEffectSoundComponent*)componentset->FindComponent(typeid(CEffectSoundComponent));

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
				else if (!strcmp(buf, "</CEffectSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CShootSoundComponent>:"))
		{
			CShootSoundComponent* component = (CShootSoundComponent*)componentset->FindComponent(typeid(CShootSoundComponent));

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
				else if (!strcmp(buf, "</CShootSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CDamageSoundComponent>:"))
		{
			CDamageSoundComponent* component = (CDamageSoundComponent*)componentset->FindComponent(typeid(CDamageSoundComponent));

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
		}
		else if (!strcmp(buf, "</Components>:"))
		{
			break;
		}
	}
}
