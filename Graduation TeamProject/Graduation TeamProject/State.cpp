#include "stdafx.h"
#include "State.h"

template<class Entity>
inline CStateMachine<Entity>::CStateMachine()
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
	return m_pStates.pop().get();
}

template<class Entity>
void CStateMachine<Entity>::Update(Entity* entity, float fElapsedTime)
{
	if (m_pStates) {
		m_pStates.top()->Execute(entity, fElapsedTime);
	}

	if (m_pGlobalState) {
		m_pGlobalState->Execute(entity, fElapsedTime);
	}
}

template<class Entity>
bool CStateMachine<Entity>::HandleMessage(const Telegram& msg) const
{
	return false;
}

template<class Entity>
void CStateMachine<Entity>::ChangeState(Entity* entity, CState<Entity>* pNewState, float fElapsedTime, bool is_popPrevious)
{
	m_pStates.top()->Exit(entity, fElapsedTime);

	if (is_popPrevious) PopState();
	PushState(pNewState);

	m_pStates.top()->Enter(entity, fElapsedTime);
}
