#pragma once
#include "..\Global\Entity.h"

class CComponent;
class CComponentSet;
class Telegram;

class CSoundPlayer : public IEntity
{
    CComponent* m_pEffectComponent = nullptr;
    CComponent* m_pShootComponent = nullptr;
    CComponent* m_pDamageComponent = nullptr;

public:
    CSoundPlayer() { }
    ~CSoundPlayer() { }
    void Update(float fElapsedTime);
    void LoadComponentFromSet(CComponentSet* componentset);
    virtual bool HandleMessage(const Telegram& msg);
};