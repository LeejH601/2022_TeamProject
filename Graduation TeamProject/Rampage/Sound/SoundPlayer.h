#pragma once
class CComponent;
class CComponentSet;
class Telegram;

class CSoundPlayer
{
    CComponent* m_pEffectComponent = nullptr;
    CComponent* m_pShootComponent = nullptr;
    CComponent* m_pDamageComponent = nullptr;

public:
    CSoundPlayer() { }
    ~CSoundPlayer() { }
    void Update(float fElapsedTime);
    void LoadComponentFromSet(CComponentSet* componentset);
};