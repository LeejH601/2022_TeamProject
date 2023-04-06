#pragma once
#include "State.h"

class CMonster;
class Idle_Monster : public CState<CMonster>
{
protected:
    std::vector<std::unique_ptr<CGameObject>>* m_pUpDownParticle = NULL;
    float m_fTime = 0.f;
public:
    DECLARE_SINGLE(Idle_Monster);
    Idle_Monster();
    ~Idle_Monster();
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);

    void SetUpDownParticleObjects(std::vector<std::unique_ptr<CGameObject>>* pParticleObjects);
};

class Damaged_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Damaged_Monster);
    Damaged_Monster();
    ~Damaged_Monster();
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Stun_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Stun_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};

class Dead_Monster : public CState<CMonster>
{
public:
    DECLARE_SINGLE(Dead_Monster);
    virtual void Enter(CMonster* monster);
    virtual void Execute(CMonster* monster, float fElapsedTime);
    virtual void Exit(CMonster* monster);
};