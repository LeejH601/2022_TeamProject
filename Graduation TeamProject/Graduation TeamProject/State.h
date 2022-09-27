#pragma once
//-----------------------------------------------------------------------------
//#include "stdafx.h"

class Telegram;
class Player;

template <class Entity>
class CState
{
public:
	CState() = default;
	virtual ~CState() = default;

	virtual void Enter(Entity* entity, float fElapsedTime) = 0;
	virtual void Execute(Entity* entity, float fElapsedTime) = 0;
	virtual void Exit(Entity* entitiy, float fElapsedTime) = 0;

	virtual bool OnMessage(Entity* entitiy, const Telegram&) = 0;
};

template <class Entity>
class CStateMachine
{
private:
	std::stack<std::shared_ptr<CState<Entity>>> m_pStates;
	std::shared_ptr<CState<Entity>> m_pGlobalState;

public:
	CStateMachine();
	virtual ~CStateMachine();

	void PushState(CState<Entity>* state);
	void SetGlobalState(CState<Entity>* state);
	CState<Entity>* PopState();

	void Update(Entity* entity, float fElapsedTime);
	bool HandleMessage(const Telegram& msg) const;

	void ChangeState(Entity* entity, CState<Entity>* pNewState,float fElapsedTime, bool is_popPrevious );
};
