#pragma once
#include "State.h"
#include "Player.h"


class IdleState : public CState<CPlayer>
{
public:
	IdleState();
	virtual ~IdleState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const CTelegram&);
};

class WalkState : public CState<CPlayer>
{
public:
	WalkState();
	virtual ~WalkState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const CTelegram&);
};

class RunState : public CState<CPlayer>
{
	bool m_bIsRun;

public:
	
	RunState();
	virtual ~RunState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const CTelegram&);
};

class EvasionState : public CState<CPlayer>
{
public:
	EvasionState();
	virtual ~EvasionState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const CTelegram&);
};

class CPlayerstate_manager
{
private:
	std::unique_ptr<CState<CPlayer>> idle;
	std::unique_ptr<CState<CPlayer>> walk;
	std::unique_ptr<CState<CPlayer>> run;
	std::unique_ptr<CState<CPlayer>> evasion;

public:
	CPlayerstate_manager() {
		idle = std::make_unique<IdleState>();
		walk = std::make_unique<WalkState>();
		run = std::make_unique<RunState>();
		evasion = std::make_unique<EvasionState>();
	}

	CState<CPlayer>* Get_state(PLAYERSTATE_TYPE type) {
		switch (type)
		{
		case PLAYERSTATE_TYPE::IDLE:
			return idle.get();
		case PLAYERSTATE_TYPE::WALK:
			return walk.get();
		case PLAYERSTATE_TYPE::RUN:
			return run.get();
		case PLAYERSTATE_TYPE::EVASION:
			return evasion.get();
		default:
			return nullptr;
		}
	}
};