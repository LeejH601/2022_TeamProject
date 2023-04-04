#include "LagdollSimulateFramework.h"
#include "..\Global\Locator.h"

CLagdollSimulateFramework::CLagdollSimulateFramework()
{
}

CLagdollSimulateFramework::~CLagdollSimulateFramework()
{
}

void CLagdollSimulateFramework::Run()
{
	while (true)
	{
		m_Timer.Tick(0.0f);
		Update(m_Timer.GetFrameTimeElapsed());
		if (m_bEnd)
			break;
	}
}

void CLagdollSimulateFramework::Update(float fTimeElapsed)
{
	static float SimulateElapsedTime = 0.0f;

	SimulateElapsedTime += fTimeElapsed;
	if (SimulateElapsedTime < 0.008f)
		return;

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("RUN LAGDOLLLLLLLLLLLLLLLLL\n"));
	OutputDebugString(pstrDebug);
	if (m_bEnd)
		return;

	if (!b_simulation) {
		Locator.GetPxScene()->simulate(SimulateElapsedTime);
		SimulateElapsedTime = 0.0f;
		b_simulation = true;
	}
	physx::PxScene* Scene = Locator.GetPxScene();
	if (Scene->fetchResults(false)) {
		//UpdateObjectArticulation();
		RegisterArticulation();
		ReadbackArticulationMatrix();
		b_simulation = false;
	}
}

void CLagdollSimulateFramework::ReadbackArticulationMatrix()
{
	//Locator.WaitApplyUpdate();
	Locator.EnterUpdateMatrix();
	for (CGameObject* obj : simulateObjects) {
		obj->updateArticulationMatrix();
	}
	Locator.LeaveUpdateMatrix();
	//Locator.SetUpdateMatrixEvent();
}

void CLagdollSimulateFramework::RequestRegisterArticulation(RequestArticulationParam& requsetParam)
{
	//Locator.WaitRequsetApply();
	Locator.EnterRequest();
	m_vRequestParams.emplace_back(requsetParam);
	Locator.LeaveRequest();
	//Locator.SetRequsetEvent();
}

void CLagdollSimulateFramework::RegisterArticulation()
{
	physx::PxScene* pScene = Locator.GetPxScene();

	//Locator.WaitRequset();
	Locator.EnterRequest();
	for (RequestArticulationParam& params : m_vRequestParams) {

		pScene->addArticulation(*params.object->m_pArticulation);

		params.object->m_pArticulationCache = params.object->m_pArticulation->createCache();
		params.object->m_nArtiCache = params.object->m_pArticulation->getCacheDataSize();

		simulateObjects.emplace_back(params.object);

		physx::PxMat44 mat = params.object->m_pArticulation->getRootGlobalPose();
		XMFLOAT4X4 rootWorld = params.object->FindFrame("root")->m_xmf4x4World;
		XMFLOAT4X4 gobworld = params.object->FindFrame("pelvis")->m_xmf4x4World;
		params.object->m_pArticulation->copyInternalStateToCache(*params.object->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
		physx::PxTransform* rootLInkTrans = &params.object->m_pArticulationCache->rootLinkData->transform;
		physx::PxMat44 tobonetrans = physx::PxMat44(*rootLInkTrans);
		tobonetrans.column3.x += rootWorld._41;
		tobonetrans.column3.y += rootWorld._42;
		tobonetrans.column3.z += rootWorld._43;
		*rootLInkTrans = physx::PxTransform(tobonetrans).getNormalized();
		params.object->m_pArticulation->applyCache(*params.object->m_pArticulationCache, physx::PxArticulationCacheFlag::eALL);
	}
	m_vRequestParams.clear();
	Locator.LeaveRequest();
	//Locator.SetRequsetApplyEvent();
}

DWORD __stdcall LagdollRun(LPVOID arg)
{
	CLagdollSimulateFramework* frameWork = (CLagdollSimulateFramework*)(arg);
	frameWork->Run();
	return 0;
}
