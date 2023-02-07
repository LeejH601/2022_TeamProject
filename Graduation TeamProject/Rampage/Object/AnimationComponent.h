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
    float m_fFrequency;
    float m_fDistance;
public:
    CShakeAnimationComponent();
    bool HandleMessage(const Telegram& msg);
};

class CStaggerAnimationComponent : public CComponent
{
    float m_fStaggerTime;
public:
    CStaggerAnimationComponent();
    bool HandleMessage(const Telegram& msg);
};