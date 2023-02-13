#pragma once
#include <fmod.h>
#include <string>
#include "..\Global\Component.h"
#include "..\Global\Entity.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.1f
#define SOUND_DEFAULT 0.2f
#define SOUND_WEIGHT 0.01f

class CSound
{
private:
    std::string m_path;
    FMOD_SOUND* m_sound;
    FMOD_CHANNEL* m_channel;
    FMOD_BOOL m_bool;

public:
    float m_volume;

public:
    CSound();
    CSound(FMOD_SYSTEM* g_sound_system, std::string path, bool loop);
    ~CSound();

    bool operator!=(const CSound& rhs);
    CSound& operator=(const CSound& rhs);
    std::string GetPath() { return m_path; }
    FMOD_SOUND* GetSound() { return m_sound; }
    FMOD_CHANNEL* GetChannel() { return m_channel; }


    void Release();
    void play(FMOD_SYSTEM* g_sound_system);
    void pause();
    void resume();
    void stop();
    void volumeUp();
    void volumeDown();
    void Update(FMOD_SYSTEM* g_sound_system);
};

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
        if(m_pSound)
            m_pSound->m_volume = m_fVolume;
    };
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

class CSoundPlayer : public IEntity
{
    CComponent* m_pEffectComponent = nullptr;
    CComponent* m_pShootComponent = nullptr;
    CComponent* m_pDamageComponent = nullptr;

public:
    void Update(float fElapsedTime);
    void LoadComponentFromSet(CComponentSet* componentset);
    virtual bool HandleMessage(const Telegram& msg);
};