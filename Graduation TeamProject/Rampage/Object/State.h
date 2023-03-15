#pragma once
#include "..\Global\stdafx.h"
#include "Object.h"
class CPlayer;

template <class entity_type>
class CState
{
public:
	virtual ~CState() {}

	virtual void Enter(entity_type*) = 0;
	virtual void Execute(entity_type*, float) = 0;
	virtual void Exit(entity_type*) = 0;
};

class Idle_Player : public CState<CPlayer>
{
public:
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk1_Player : public CState<CPlayer>
{
public:
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk2_Player : public CState<CPlayer>
{
public:
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk3_Player : public CState<CPlayer>
{
public:
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Run_Player : public CState<CPlayer>
{
public:
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};