#pragma once
#include "State.h"

class CMonster;
class Idle_Monster : public CState<CMonster>
{
private:
    float m_fMaxIdleTime = 1.0f;
public:
    DECLARE_SINGLE(Idle_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Wander_Monster : public CState<CMonster>
{
private:
    float m_fMaxWanderTime = 1.0f;
public:
    DECLARE_SINGLE(Wander_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Chasing_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Chasing_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Attack_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Attack_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Damaged_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Damaged_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Stun_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Stun_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Animate(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};