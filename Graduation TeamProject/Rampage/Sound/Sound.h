#pragma once
#include <fmod.h>
#include <string>

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.1f
#define SOUND_DEFAULT 0.1f
#define SOUND_WEIGHT 0.01f

class CSound
{
private:
    std::string m_path;
    FMOD_SOUND* m_sound;
    FMOD_CHANNEL* m_channel;
    float m_volume;
    FMOD_BOOL m_bool;
public:
    CSound();
    CSound(FMOD_SYSTEM* g_sound_system, std::string path, bool loop);
    ~CSound();
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