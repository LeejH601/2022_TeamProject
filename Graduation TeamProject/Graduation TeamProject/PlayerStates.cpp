#include "stdafx.h"
#include "PlayerStates.h"
#include "Locator.h"

IdleState::IdleState()
{
}

IdleState::~IdleState()
{

}

void IdleState::Enter(CPlayer* entity, float fElapsedTime)
{
	printf("this is IdleState Enter\n");
}

void IdleState::Execute(CPlayer* entity, float fElapsedTime)
{
	static int count = 0;
	printf("this is IdleState Excute\n");
	if (count >= 1) {
		count = 0;
		entity->Get_FSM()->ChangeState(entity, Locator.Get_PlayerState(PLAYERSTATE_TYPE::WALK), 0.0f, false);
	}
	count++;
}

void IdleState::Exit(CPlayer* entitiy, float fElapsedTime)
{
	printf("this is IdleState Exit\n");
}

bool IdleState::OnMessage(CPlayer* entitiy, const CTelegram&)
{
	return false;
}

WalkState::WalkState()
{
}

WalkState::~WalkState()
{
}

void WalkState::Enter(CPlayer* entity, float fElapsedTime)
{
	printf("this is WalkState Enter\n");
}

void WalkState::Execute(CPlayer* entity, float fElapsedTime)
{
	static int count = 0;
	printf("this is WalkState Execute\n");
	if (count >= 1) {
		count = 0;
		entity->Get_FSM()->ChangeState(entity, Locator.Get_PlayerState(PLAYERSTATE_TYPE::RUN), 0.0f, false);
	}
	count++;
}

void WalkState::Exit(CPlayer* entitiy, float fElapsedTime)
{
	printf("this is WalkState Exit\n");
}

bool WalkState::OnMessage(CPlayer* entitiy, const CTelegram&)
{
	return false;
}

RunState::RunState()
{
}

RunState::~RunState()
{
}

void RunState::Enter(CPlayer* entity, float fElapsedTime)
{
	printf("this is RunState Enter\n");
}

void RunState::Execute(CPlayer* entity, float fElapsedTime)
{
	static int count = 0;
	printf("this is RunState Execute\n");
	if (count >= 1) {
		count = 0;
		entity->Get_FSM()->ChangeState(entity, nullptr, 0.0f, true);
	}
	count++;
}

void RunState::Exit(CPlayer* entitiy, float fElapsedTime)
{
	printf("this is RunState Exit\n");
}

bool RunState::OnMessage(CPlayer* entitiy, const CTelegram&)
{
	return false;
}

EvasionState::EvasionState()
{
}

EvasionState::~EvasionState()
{
}

void EvasionState::Enter(CPlayer* entity, float fElapsedTime)
{
	printf("this is EvasionState Enter\n");
}

void EvasionState::Execute(CPlayer* entity, float fElapsedTime)
{
	printf("this is EvasionState Execute\n");
}

void EvasionState::Exit(CPlayer* entitiy, float fElapsedTime)
{
	printf("this is EvasionState Exit\n");
}

bool EvasionState::OnMessage(CPlayer* entitiy, const CTelegram&)
{
	return false;
}
