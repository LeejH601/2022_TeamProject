#pragma once
#include "State.h"

class CPlayer;

class IdleState : public CState<CPlayer>
{
public:
	IdleState();
	~IdleState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const Telegram&);
};

class WalkState : public CState<CPlayer>
{
	WalkState();
	~WalkState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const Telegram&);
};

class RunState : public CState<CPlayer>
{

	RunState();
	~RunState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const Telegram&);
};

class EvasionState : public CState<CPlayer>
{
	EvasionState();
	~EvasionState();

	virtual void Enter(CPlayer* entity, float fElapsedTime);
	virtual void Execute(CPlayer* entity, float fElapsedTime);
	virtual void Exit(CPlayer* entitiy, float fElapsedTime);

	virtual bool OnMessage(CPlayer* entitiy, const Telegram&);
};

