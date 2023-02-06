#pragma once
#include "Sound.h"
#include "..\Global\Component.h"

class CSoundComponent : public CComponent
{
protected:
    FMOD_SYSTEM* g_sound_system;

    CSound* m_pSound = nullptr;
    float m_fDelay = 0.0f;
    float m_fCurrDelayed = 0.0f;
    float* m_fVolume = nullptr;

public:

    virtual void Update(float fElapsedTime);
    virtual void Reset();
    void SetSound(const std::string path);
};

class CEffectSoundComponent : public CSoundComponent
{
public:
    CEffectSoundComponent(FMOD_SYSTEM* sound_system);
    bool HandleMessage(const Telegram& msg);
};

class CShootSoundComponent : public CSoundComponent
{
public:
    CShootSoundComponent(FMOD_SYSTEM* sound_system);
    bool HandleMessage(const Telegram& msg);
};

class CDamageSoundComponent : public CSoundComponent
{
public:
    CDamageSoundComponent(FMOD_SYSTEM* sound_system);
    bool HandleMessage(const Telegram& msg);
};