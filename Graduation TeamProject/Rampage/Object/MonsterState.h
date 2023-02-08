#pragma once
#include "State.h"

class CMonster;
class Idle_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Idle_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
    virtual bool OnMessage(CMonster* monster, const Telegram& msg);
};

class Damaged_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Damaged_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
    virtual bool OnMessage(CMonster* monster, const Telegram& msg);
};