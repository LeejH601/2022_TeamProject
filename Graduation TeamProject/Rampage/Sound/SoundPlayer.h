#pragma once
#include "..\Global\stdafx.h"
#include "Sound.h"

class CSound;
class CSoundPlayer
{
    struct SoundPlayInfo {
        CSound* pSound;
        float fDelay;
    };
    std::vector<SoundPlayInfo> m_vSounds;
    std::vector<FMOD_CHANNEL*> m_vPlayingSounds;
    
    void PlaySound(FMOD_SYSTEM* g_sound_system, SoundPlayInfo sound_play_info);
public:
    CSoundPlayer() { }
    ~CSoundPlayer() { }

    void RegisterSound(CSound* pSound, float fDelay = 0.0f);
    void PlaySounds(FMOD_SYSTEM* g_sound_system);
    void StopSounds();
};