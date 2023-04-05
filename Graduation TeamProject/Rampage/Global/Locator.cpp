#include "Locator.h"
#include "Component.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"

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
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(4); // �� ������ �Ķ���Ͱ� �ݵ�� �ʿ���.
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; // ���� � ������ �ϴ� ���� �߰������� �����غ� �ʿ䰡 ����.
	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f * 3.0f, 0.0f);

	m_pPxScene = m_pPhysics->createScene(SceneDesc);

	pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return true;
}