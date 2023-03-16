#pragma once
#include "stdafx.h"
#include "Global.h"
#include "Camera.h"
#include "..\Object\State.h"

template <class entity_type> class CState;
class CCamera;
class CGameObject;
class CComponentSet;
class CPlayer;
class CGameTimer;

typedef std::pair<size_t, std::shared_ptr<CState<CPlayer>>> PlayerStatePair;

class Comp_PlayerState
{
public:
	bool operator()(const PlayerStatePair& lhs, const PlayerStatePair& rhs) const {
		return lhs.first < rhs.first;
	}
};

class Statecomp {
public:
	using is_transparent = size_t;

	template<class t1, class t2>
	constexpr auto operator()(t1&& lhs, t2&& rhs) const {
		return static_cast<t1&&>(lhs) < typeid(*(static_cast<t2&&>(rhs).get())).hash_code();
	};
};

enum class MOUSE_CUROSR_MODE {
	THIRD_FERSON_MODE,
	FLOATING_MODE,
};

class CLocator
{
	std::set<PlayerStatePair, Comp_PlayerState> m_sPlayerStateSet;

	physx::PxFoundation* m_pFoundation;
	physx::PxPhysics* m_pPhysics;
	physx::PxScene* m_pPxScene;
	physx::PxPvd* m_pPxPvd;
	physx::PxPvdSceneClient* pvdClient = nullptr;

	CGameTimer* m_pTimer;
	
	PlayerStatePair statedummy;

	MOUSE_CUROSR_MODE m_eMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;

public:
	CLocator() = default;
	~CLocator();

	bool Init();

	void OnChangeScene(SCENE_TYPE scene_type);

	CState<CPlayer>* GetPlayerState(const std::type_info& type);

	void SetPlayerState(std::shared_ptr<CState<CPlayer>>& state);

	void SetTimer(CGameTimer* timer) { m_pTimer = timer; };
	CGameTimer* GetTimer() { return m_pTimer; };

	physx::PxFoundation* GetPxFoundation() { return m_pFoundation; };
	physx::PxPhysics* GetPxPhysics() { return m_pPhysics; };
	physx::PxScene* GetPxScene() { return m_pPxScene; };

	void SetMouseCursorMode(MOUSE_CUROSR_MODE mode) { m_eMouseCursorMode = mode; };
	MOUSE_CUROSR_MODE GetMouseCursorMode() { return m_eMouseCursorMode; };
};

extern CLocator Locator;

