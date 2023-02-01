#pragma once
#include "stdafx.h"

class CCamera;
class CGameObject;
class CComponentSet;

typedef std::pair<int, std::shared_ptr<CComponentSet>> CoptSetPair;

class Comp_ComponentSet
{
public:
	bool operator()(const CoptSetPair& lhs, const CoptSetPair& rhs) const {
		return lhs.first < rhs.first;
	}
};


class CLocator
{
	std::shared_ptr<CCamera> m_pSimulaterCamera;
	std::shared_ptr<CGameObject> m_pSimulaterPlayer;
	std::shared_ptr<CGameObject> m_pMainPlayer;

	std::set<CoptSetPair, Comp_ComponentSet> m_sComponentSets;

	CoptSetPair dummy;

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
};

extern CLocator Locator;

