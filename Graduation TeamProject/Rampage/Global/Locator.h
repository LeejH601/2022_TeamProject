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
	ID3D12Device* m_pd3dDevice;
	physx::PxFoundation* m_pFoundation;
	physx::PxPhysics* m_pPhysics;
	physx::PxScene* m_pPxScene;
	physx::PxPvd* m_pPxPvd;
	physx::PxPvdSceneClient* pvdClient = nullptr;

	CGameTimer* m_pTimer;

	MOUSE_CUROSR_MODE m_eMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;

public:
	CLocator() = default;
	~CLocator();

	bool Init();
	void SetTimer(CGameTimer* timer) { m_pTimer = timer; };
	CGameTimer* GetTimer() { return m_pTimer; };

	physx::PxFoundation* GetPxFoundation() { return m_pFoundation; };
	physx::PxPhysics* GetPxPhysics() { return m_pPhysics; };
	physx::PxScene* GetPxScene() { return m_pPxScene; };

	void SetDevice(ID3D12Device* device) { m_pd3dDevice = device; };
	ID3D12Device* GetDevice() { return m_pd3dDevice; };

	void SetMouseCursorMode(MOUSE_CUROSR_MODE mode) { m_eMouseCursorMode = mode; };
	MOUSE_CUROSR_MODE GetMouseCursorMode() { return m_eMouseCursorMode; };
};

extern CLocator Locator;

