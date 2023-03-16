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
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(8); // 이 세가지 파라미터가 반드시 필요함.
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; // 각각 어떤 역할을 하는 지는 추가적으로 조사해볼 필요가 있음.
	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f * 1.0f, 0.0f);

	m_pPxScene = m_pPhysics->createScene(SceneDesc);

	pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
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
