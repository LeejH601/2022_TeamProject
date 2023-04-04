#pragma once
#include "stdafx.h"
#include "Global.h"
#include "Camera.h"
#include "..\Object\State.h"

class CCamera;
class CGameObject;
class CComponentSet;
class CGameTimer;

enum class MOUSE_CUROSR_MODE {
	THIRD_FERSON_MODE,
	FLOATING_MODE,
};

class CLocator
{
	physx::PxFoundation* m_pFoundation;
	physx::PxPhysics* m_pPhysics;
	physx::PxScene* m_pPxScene;
	physx::PxPvd* m_pPxPvd;
	physx::PxPvdSceneClient* pvdClient = nullptr;

	CGameTimer* m_pTimer;

	MOUSE_CUROSR_MODE m_eMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;

	HANDLE hRequsetApplyEvent;
	HANDLE hRequsetEvent;
	HANDLE hPxSimulateEvent;
	HANDLE hPxFetchEvent;
	HANDLE hUpdateMatrixEvent;
	HANDLE hApplyUpdateMatrixEvent;

	CRITICAL_SECTION Cs_UpdateMatrix;
	CRITICAL_SECTION Cs_Request;
	CRITICAL_SECTION Cs_Apply;
	HANDLE hLagdollThread;

public:
	CLocator() = default;
	~CLocator();

	bool Init();
	void SetTimer(CGameTimer* timer) { m_pTimer = timer; };
	CGameTimer* GetTimer() { return m_pTimer; };

	physx::PxFoundation* GetPxFoundation() { return m_pFoundation; };
	physx::PxPhysics* GetPxPhysics() { return m_pPhysics; };
	physx::PxScene* GetPxScene() { return m_pPxScene; };

	void SetMouseCursorMode(MOUSE_CUROSR_MODE mode) { m_eMouseCursorMode = mode; };
	MOUSE_CUROSR_MODE GetMouseCursorMode() { return m_eMouseCursorMode; };

	void SetRequsetEvent() { SetEvent(hRequsetEvent); };
	void SetRequsetApplyEvent() { SetEvent(hRequsetApplyEvent); };
	void SetSimluateEvent() { SetEvent(hPxSimulateEvent); };
	void SetFetchEvent() { SetEvent(hPxFetchEvent); };
	void SetUpdateMatrixEvent() { SetEvent(hUpdateMatrixEvent); };
	void SetApplyUpdateEvent() { SetEvent(hApplyUpdateMatrixEvent); };

	void WaitRequset() { WaitForSingleObject(hRequsetEvent, INFINITE); };
	void WaitRequsetApply() { WaitForSingleObject(hRequsetApplyEvent, INFINITE); };
	void WaitSimluate() { WaitForSingleObject(hPxSimulateEvent, INFINITE); };
	void WaitFetch() { WaitForSingleObject(hPxFetchEvent, INFINITE); };
	void WaitUpdateMatrix() { WaitForSingleObject(hUpdateMatrixEvent, INFINITE); };
	void WaitApplyUpdate() { WaitForSingleObject(hApplyUpdateMatrixEvent, INFINITE); };

	void EnterRequest() { EnterCriticalSection(&Cs_Request); };
	void LeaveRequest() { LeaveCriticalSection(&Cs_Request); };
	
	void EnterUpdateMatrix() { EnterCriticalSection(&Cs_UpdateMatrix); };
	void LeaveUpdateMatrix() { LeaveCriticalSection(&Cs_UpdateMatrix); };

	void SetLagdollThreadHandle(HANDLE hThread) { hLagdollThread = hThread; };
	HANDLE GetLagdollThreadHandl() { return hLagdollThread; };
};

extern CLocator Locator;

