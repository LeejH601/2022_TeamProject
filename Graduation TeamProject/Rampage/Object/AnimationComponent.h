#pragma once
#include "..\Global\Component.h"
class CDamageAnimationComponent : public CComponent
{
    float m_fMaxDistance = 5.0f;
    float m_fSpeed = 100.0f;
public:
    static std::shared_ptr<CDamageAnimationComponent> GetInst()
    {
        static std::shared_ptr<CDamageAnimationComponent> m_pInst = std::make_shared<CDamageAnimationComponent>();
        return m_pInst;
    }
    CDamageAnimationComponent();
    float& GetMaxDistance() { return m_fMaxDistance; }
    float& GetSpeed() { return m_fSpeed; }
    float GetDamageDistance(float t);
    bool HandleMessage(const Telegram& msg);
};

class CShakeAnimationComponent : public CComponent
{
    float m_fDistance = 0.15f;
    float m_fFrequency = 0.05f;
public:
    static std::shared_ptr<CShakeAnimationComponent> GetInst()
    {
        static std::shared_ptr<CShakeAnimationComponent> m_pInst = std::make_shared<CShakeAnimationComponent>();
        return m_pInst;
    }
    CShakeAnimationComponent();
    float& GetDistance() { return m_fDistance; }
    float& GetFrequency() { return m_fFrequency; }
    float GetShakeDistance(float t);
    bool HandleMessage(const Telegram& msg);
};

class CStunComponent : public CComponent
{
    float m_fStunTime = 0.5f;
public:
    static std::shared_ptr<CStunComponent> GetInst()
    {
        static std::shared_ptr<CStunComponent> m_pInst = std::make_shared<CStunComponent>();
        return m_pInst;
    }
    CStunComponent();
    float& GetStunTime() { return m_fStunTime; }
    bool HandleMessage(const Telegram& msg);
};