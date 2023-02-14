#pragma once
#include "..\Global\Component.h"
class CDamageAnimationComponent : public CComponent
{
    float m_fMaxDistance = 5.0f;
    float m_fSpeed = 20.0f;
public:
    DECLARE_SINGLE(CDamageAnimationComponent);
    CDamageAnimationComponent();
    float GetMaxEistance() { return m_fMaxDistance; }
    float GetDamageDistance(float t);
    bool HandleMessage(const Telegram& msg);
};

class CShakeAnimationComponent : public CComponent
{
    float m_fDistance = 0.15f;
    float m_fFrequency = 0.05f;
public:
    DECLARE_SINGLE(CShakeAnimationComponent);
    CShakeAnimationComponent();
    float GetShakeDistance(float t);
    bool HandleMessage(const Telegram& msg);
};

class CStunAnimationComponent : public CComponent
{
    float m_fStunTime = 0.5f;
public:
    DECLARE_SINGLE(CStunAnimationComponent);
    CStunAnimationComponent();
    float GetStunTime() { return m_fStunTime; }
    bool HandleMessage(const Telegram& msg);
};