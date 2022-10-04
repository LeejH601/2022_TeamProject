#pragma once
//-----------------------------------------------------------------------------
//#include "stdafx.h"
#include "State.h"

class Telegram {
	int a = 0;
};

template <class Entity>
class CState
{
public:
	CState() {};
	virtual ~CState() {};

	virtual void Enter(Entity* entity, float fElapsedTime) = 0;
	virtual void Execute(Entity* entity, float fElapsedTime) = 0;
	virtual void Exit(Entity* entitiy, float fElapsedTime) = 0;

	virtual bool OnMessage(Entity* entitiy, const Telegram&) = 0;
};



template <class Entity>
class CStateMachine
{
private:
	std::stack<CState<Entity>*> m_pStates;
	std::shared_ptr<CState<Entity>> m_pGlobalState = nullptr;

public:
	CStateMachine();
	virtual ~CStateMachine();

	void PushState(CState<Entity>* state);
	void SetGlobalState(CState<Entity>* state);
	CState<Entity>* PopState();

	void Update(Entity* entity, float fElapsedTime);
	bool HandleMessage(Entity* entity, const Telegram& msg) const;

	void ChangeState(Entity* entity, CState<Entity>* pNewState, float fElapsedTime, bool is_popPrevious);
	void processingKeyEvent(Entity* entity, char* key, float fElapsedTime);
};


//class CStateMachine
//{
//private:
//	std::stack<std::shared_ptr<CState<Player>>> m_pStates;
//	std::shared_ptr<CState<Player>> m_pGlobalState = nullptr;
//
//public:
//	CStateMachine();
//	virtual ~CStateMachine();
//
//	void PushState(CState<Player>* state);
//	void SetGlobalState(CState<Player>* state);
//	CState<Player>* PopState();
//
//	void Update(Player* entity, float fElapsedTime);
//	bool HandleMessage(const Telegram& msg) const;
//
//	void ChangeState(Player* entity, CState<Player>* pNewState, float fElapsedTime, bool is_popPrevious);
//};


template<class Entity>
CStateMachine<Entity>::CStateMachine()
{
}

template<class Entity>
CStateMachine<Entity>::~CStateMachine()
{
}

template<class Entity>
void CStateMachine<Entity>::PushState(CState<Entity>* state)
{
	m_pStates.push(state);
}

template<class Entity>
void CStateMachine<Entity>::SetGlobalState(CState<Entity>* state)
{
	m_pGlobalState = state;
}

template<class Entity>
CState<Entity>* CStateMachine<Entity>::PopState()
{
	if (!m_pStates.empty()) {
		CState<Entity>* state = m_pStates.top();
		m_pStates.pop();
		return state;
	}
	return nullptr;
}

template<class Entity>
void CStateMachine<Entity>::Update(Entity* entity, float fElapsedTime)
{
	if (!m_pStates.empty()) {
		m_pStates.top()->Execute(entity, fElapsedTime);
	}

	if (m_pGlobalState) {
		m_pGlobalState->Execute(entity, fElapsedTime);
	}
}

template<class Entity>
bool CStateMachine<Entity>::HandleMessage(Entity* entity, const Telegram& msg) const
{
	return m_pStates->OnMessage(entity, msg);
}

template<class Entity>
void CStateMachine<Entity>::ChangeState(Entity* entity, CState<Entity>* pNewState, float fElapsedTime, bool is_popPrevious)
{
	if (!m_pStates.empty())
		m_pStates.top()->Exit(entity, fElapsedTime);

	if (is_popPrevious) PopState();
	if (pNewState)
		PushState(pNewState);

	m_pStates.top()->Enter(entity, fElapsedTime);
	return;
}

template<class Entity>
inline void CStateMachine<Entity>::processingKeyEvent(Entity* entity, char* key, float fElapsedTime)
{
	m_pStates.top();
}
