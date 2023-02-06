#pragma once
#include "Sound.h"
#include <vector>
#include <algorithm>
#include <ctime>

class CSoundManager {
private:
    FMOD_SYSTEM* g_sound_system;
    std::vector<CSound> m_Sounds;
public:
    DECLARE_SINGLE(CSoundManager);

    CSoundManager();
    ~CSoundManager();

    void Init();
    void Release();
    FMOD_SYSTEM* GetSoundSystem() { return g_sound_system; };

    void RegisterSound(std::string path, bool loop);
    std::vector<CSound>::iterator FindSound(const std::string path);
    void PlaySound(std::string path);
    void PauseSound(std::string path);
    void ResumeSound(std::string path);
    void StopSound(std::string path);
    void VolumeUp(std::string path);
    void VolumeDown(std::string path);
    void UpdateSound(std::string path);
};