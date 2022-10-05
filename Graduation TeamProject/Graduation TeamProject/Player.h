#pragma once
#include "State.h"
#include "Telegram.h"

class CPlayer {
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	bool m_bIsDownEvasion;
	bool m_bIsDownRun;

public:
	CPlayer() {
		m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>();
	};

	void Update() {
		m_pStateMachine->Update(this, 0.0f);
	}

	CStateMachine<CPlayer>* Get_FSM() { return m_pStateMachine.get(); };

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};