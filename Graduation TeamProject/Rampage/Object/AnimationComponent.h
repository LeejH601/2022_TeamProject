#pragma once
#include "..\Global\Component.h"
class CDamageAnimationComponent : public CComponent
{
    float m_fDelay = 0.0f;
    float m_fSpeed = 0.0f;
public:
    CDamageAnimationComponent();
    bool HandleMessage(const Telegram& msg);
};

class CShakeAnimationComponent : public CComponent
{
    float m_fDistance = 0.5f;
    float m_fFrequency = 0.2f;
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