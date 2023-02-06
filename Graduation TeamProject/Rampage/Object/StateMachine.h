#pragma once
#include "State.h"

template <class entity_type>
class CStateMachine
{
private:
	entity_type* m_pOwner;

	CState<entity_type>* m_pCurrentState;

	CState<entity_type>* m_pPreviousState;

	CState<entity_type>* m_pGlobalState;

public:

	CStateMachine(entity_type* owner) :m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}

	virtual ~CStateMachine() {}

	void SetCurrentState(CState<entity_type>* s) { m_pCurrentState = s; }
	void SetGlobalState(CState<entity_type>* s) { m_pGlobalState = s; }
	void SetPreviousState(CState<entity_type>* s) { m_pPreviousState = s; }

	void  Update(float fElapsedTime)const
	{
		if (m_pGlobalState)   m_pGlobalState->Execute(m_pOwner, fElapsedTime);
		if (m_pCurrentState) m_pCurrentState->Execute(m_pOwner, fElapsedTime);
	}

	bool  HandleMessage(const Telegram& msg)const
	{
		if (m_pCurrentState && m_pCurrentState->OnMessage(m_pOwner, msg))
		{
			return true;
		}

		if (m_pGlobalState && m_pGlobalState->OnMessage(m_pOwner, msg))
		{
			return true;
		}

		return false;
	}

	//change to a new state
	void  ChangeState(CState<entity_type>* pNewState)
	{
		assert(pNewState && "<StateMachine::ChangeState>:trying to assign null state to current");

		m_pPreviousState = m_pCurrentState;
		m_pCurrentState->Exit(m_pOwner);
		m_pCurrentState = pNewState;
		m_pCurrentState->Enter(m_pOwner);
	}

	void  RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}

	bool  isInState(const CState<entity_type>& st) const {
		if (typeid(*m_pCurrentState) == typeid(st))
			return true;
		return false;
	}

	CState<entity_type>* GetCurrentState()  const { return m_pCurrentState; }
	CState<entity_type>* GetGlobalState()   const { return m_pGlobalState; }
	CState<entity_type>* GetPreviousState() const { return m_pPreviousState; }

	std::string GetNameOfCurrentState() const
	{
		std::string s(typeid(*m_pCurrentState).name());

		//remove the 'class ' part from the front of the string
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		return s;
	}
};