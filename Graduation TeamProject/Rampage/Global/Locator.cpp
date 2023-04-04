#include "Locator.h"
#include "Component.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"

CLocator::~CLocator()
{
	CloseHandle(hRequsetEvent);
	CloseHandle(hRequsetApplyEvent);
	CloseHandle(hPxSimulateEvent);
	CloseHandle(hPxFetchEvent);
	CloseHandle(hUpdateMatrixEvent);
	CloseHandle(hApplyUpdateMatrixEvent);

	DeleteCriticalSection(&Cs_Request);
	DeleteCriticalSection(&Cs_UpdateMatrix);
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

	hRequsetEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("RequestEvent"));
	hRequsetApplyEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("RequestApplyEvent"));
	hPxSimulateEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PxSimulateEvent"));
	hPxFetchEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PxFetchEvent"));
	hUpdateMatrixEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("UpdateMatrixEvent"));
	hApplyUpdateMatrixEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ApplyUpdateMatrixEvent"));

	SetEvent(hRequsetEvent);
	SetEvent(hApplyUpdateMatrixEvent);

	InitializeCriticalSection(&Cs_Request);
	InitializeCriticalSection(&Cs_UpdateMatrix);

	return true;
}