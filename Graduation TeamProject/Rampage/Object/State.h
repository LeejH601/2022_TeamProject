#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\MessageDispatcher.h"
#include "Object.h"
class CPlayer;

template <class entity_type>
class CState
{
protected:
    std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
public:
	virtual ~CState() {}

	virtual void Enter(entity_type*) = 0;
	virtual void Execute(entity_type*, float) = 0;
	virtual void Exit(entity_type*) = 0;
};

class Idle_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Idle_Player);
    Idle_Player();
    ~Idle_Player();
    
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk1_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk1_Player);
    Atk1_Player();
    ~Atk1_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk2_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk2_Player);
    Atk2_Player();
    ~Atk2_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk3_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk3_Player);
    Atk3_Player();
    ~Atk3_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Run_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Run_Player);
    Run_Player();
    ~Run_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};