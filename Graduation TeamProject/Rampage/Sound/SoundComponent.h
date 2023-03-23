#pragma once
#include "Sound.h"
#include "..\Global\Component.h"

class CSoundComponent : public CComponent
{
protected:
    FMOD_SYSTEM* g_sound_system;

    CSound* m_pSound = nullptr;
public:
    unsigned int m_nSoundNumber = 0;
    float m_fDelay = 0.0f;
    float m_fCurrDelayed = 0.0f;
    float m_fVolume = 0.0f;

public:

    virtual void Update(float fElapsedTime);
    virtual void Reset();
    void SetSound(CSound* sound);
    void UpdateVolume() {
        if (m_pSound)
            m_pSound->m_volume = m_fVolume;
    };
};

class CEffectSoundComponent : public CSoundComponent
{
public:
    CEffectSoundComponent(FMOD_SYSTEM* sound_system);
    virtual void Reset();
};

class CShootSoundComponent : public CSoundComponent
{
public:
    CShootSoundComponent(FMOD_SYSTEM* sound_system);
    virtual void Reset();
};

class CDamageSoundComponent : public CSoundComponent
{
public:
    CDamageSoundComponent(FMOD_SYSTEM* sound_system);
    virtual void Reset();
};

