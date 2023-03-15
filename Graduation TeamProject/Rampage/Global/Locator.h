#pragma once
#include "stdafx.h"
#include "Global.h"
#include "Camera.h"
#include "..\Sound\SoundPlayer.h"
#include "..\Object\State.h"

template <class entity_type> class CState;
class CCamera;
class CGameObject;
class CComponentSet;
class CPlayer;
class CGameTimer;

typedef std::pair<int, std::shared_ptr<CComponentSet>> CoptSetPair;

class Comp_ComponentSet
{
public:
	bool operator()(const CoptSetPair& lhs, const CoptSetPair& rhs) const {
		return lhs.first < rhs.first;
	}
};

class DataLoader
{
	std::string file_path = "Data\\Component";
	std::string file_ext = ".bin";

public:
	void SaveComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets);
	void LoadComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets);

	void SaveComponentSet(FILE* pInFile, CComponentSet* componentset);
	void LoadComponentSet(FILE* pInFile, CComponentSet* componentset);
};


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
	std::set<CoptSetPair, Comp_ComponentSet> m_sComponentSets;
	std::set<PlayerStatePair, Comp_PlayerState> m_sPlayerStateSet;

	std::unique_ptr<CSoundPlayer> m_pSoundPlayer = NULL;

	physx::PxFoundation* m_pFoundation;
	physx::PxPhysics* m_pPhysics;
	physx::PxScene* m_pPxScene;
	physx::PxPvd* m_pPxPvd;
	physx::PxPvdSceneClient* pvdClient = nullptr;

	CGameTimer* m_pTimer;

	CoptSetPair dummy;
	PlayerStatePair statedummy;

	MOUSE_CUROSR_MODE m_eMouseCursorMode = MOUSE_CUROSR_MODE::FLOATING_MODE;

public:
	CLocator() = default;
	~CLocator();

	bool Init();

	void OnChangeScene(SCENE_TYPE scene_type);

	CComponentSet* GetComponentSet(int num)
	{
		dummy.first = num;

		std::set<CoptSetPair, Comp_ComponentSet>::iterator it = m_sComponentSets.find(dummy);
		if (it == m_sComponentSets.end())
			return nullptr;

		return it->second.get();
	}
	void SetComponentSet(std::shared_ptr<CComponentSet>& pComponentSet) {
		static int ComponentSets_Num = 0;
		std::shared_ptr<CComponentSet> pSet = pComponentSet;
		CoptSetPair pair = std::make_pair(ComponentSets_Num++, pSet);
		m_sComponentSets.insert(pair);
	}

	CState<CPlayer>* GetPlayerState(const std::type_info& type);

	void SetPlayerState(std::shared_ptr<CState<CPlayer>>& state);

	CSoundPlayer* GetSoundPlayer() { return m_pSoundPlayer.get(); };

	void SetTimer(CGameTimer* timer) { m_pTimer = timer; };
	CGameTimer* GetTimer() { return m_pTimer; };

	std::set<CoptSetPair, Comp_ComponentSet>& GetComponentSetRoot() { return m_sComponentSets; };

	physx::PxFoundation* GetPxFoundation() { return m_pFoundation; };
	physx::PxPhysics* GetPxPhysics() { return m_pPhysics; };
	physx::PxScene* GetPxScene() { return m_pPxScene; };

	void SetMouseCursorMode(MOUSE_CUROSR_MODE mode) { m_eMouseCursorMode = mode; };
	MOUSE_CUROSR_MODE GetMouseCursorMode() { return m_eMouseCursorMode; };
};

extern CLocator Locator;

