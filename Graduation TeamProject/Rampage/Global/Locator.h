#pragma once
#include "stdafx.h"

template <class entity_type> class CState;
class CCamera;
class CGameObject;
class CSoundPlayer;
class CComponentSet;
class CPlayer;

typedef std::pair<int, std::shared_ptr<CComponentSet>> CoptSetPair;

class Comp_ComponentSet
{
public:
	bool operator()(const CoptSetPair& lhs, const CoptSetPair& rhs) const {
		return lhs.first < rhs.first;
	}
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

class CLocator
{
	std::shared_ptr<CCamera> m_pSimulaterCamera;
	std::shared_ptr<CGameObject> m_pSimulaterPlayer;
	std::shared_ptr<CGameObject> m_pMainPlayer;

	std::set<CoptSetPair, Comp_ComponentSet> m_sComponentSets;
	std::set<PlayerStatePair, Comp_PlayerState> m_sPlayerStateSet;

	std::shared_ptr<CSoundPlayer> m_pSoundPlayer;

	CoptSetPair dummy;
	PlayerStatePair statedummy;

public:
	CLocator() = default;
	~CLocator() = default;

	bool Init();

	CCamera* GetSimulaterCamera() { return m_pSimulaterCamera.get(); };
	std::shared_ptr<CCamera>& GetSimulaterCameraWithShared() { return m_pSimulaterCamera; };
	void SetSimulaterCamera(std::shared_ptr<CCamera> pCamera) { m_pSimulaterCamera = pCamera; };

	CGameObject* GetSimulaterPlayer() { return m_pSimulaterPlayer.get(); };
	void SetSimulaterPlayer(std::shared_ptr<CGameObject> pPlayer) { m_pSimulaterPlayer = pPlayer; };

	CGameObject* GetMainPlayer() { return m_pMainPlayer.get(); };
	void SetMainPlayer(std::shared_ptr<CGameObject> pPlayer) { m_pMainPlayer = pPlayer; };

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
};

extern CLocator Locator;

