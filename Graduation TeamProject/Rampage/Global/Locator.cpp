#include "Locator.h"
#include "Component.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"
#include "..\Object\BillBoardComponent.h"
#include "..\Object\ParticleComponent.h"

CLocator::~CLocator()
{
}

bool CLocator::Init()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	m_pPxPvd = physx::PxCreatePvd(*m_pFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
	m_pPxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	bool recordMemoryAllocations = true;

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(1.0f, 10.0f), recordMemoryAllocations, m_pPxPvd);

	physx::PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(8); // �� ������ �Ķ���Ͱ� �ݵ�� �ʿ���.
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; // ���� � ������ �ϴ� ���� �߰������� �����غ� �ʿ䰡 ����.
	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f * 1.0f, 0.0f);

	m_pPxScene = m_pPhysics->createScene(SceneDesc);

	pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);


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

	m_pSoundPlayer = std::make_unique<CSoundPlayer>();

	return true;
}

void CLocator::OnChangeScene(SCENE_TYPE scene_type)
{
	/*switch (scene_type)
	{
	case SCENE_TYPE::LOBBY_SCENE:
		break;
	case SCENE_TYPE::MAIN_SCENE:
		for (auto& [num, componentSet] : m_sComponentSets) {
			CCameraMover* mover = (CCameraMover*)componentSet->FindComponent(typeid(CCameraMover));
			mover->SetCamera(m_pMainSceneCamera.get());

			CCameraMover* shaker = (CCameraMover*)componentSet->FindComponent(typeid(CCameraShaker));
			shaker->SetCamera(m_pMainSceneCamera.get());

			CCameraMover* zoomer = (CCameraMover*)componentSet->FindComponent(typeid(CCameraZoomer));
			zoomer->SetCamera(m_pMainSceneCamera.get());
		}
		break;
	}*/
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
