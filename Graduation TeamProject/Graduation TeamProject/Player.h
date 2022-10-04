#pragma once
#include "State.h"

class CPlayer {
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

public:
	CPlayer() {
		m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>();
	};

	void Update() {
		m_pStateMachine->Update(this, 0.0f);
	}

	CStateMachine<CPlayer>* Get_FSM() { return m_pStateMachine.get(); };

	void processingKeyEvent(char key) {
		//m_pStateMachine
	};
};