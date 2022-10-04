#pragma once
#include "PlayerStates.h"

class CLocator
{
private:
	std::unique_ptr<CPlayerstate_manager> m_pPlayerStateManager;

public:
	CLocator() {
		m_pPlayerStateManager = std::make_unique<CPlayerstate_manager>();
	};

	CState<CPlayer>* Get_PlayerState(PLAYERSTATE_TYPE type) {
		return m_pPlayerStateManager->Get_state(type);
	}
};

extern CLocator Locator;