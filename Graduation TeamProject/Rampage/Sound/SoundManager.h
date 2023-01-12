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
    static CSoundManager* GetInst()
    {
        static CSoundManager m_pInst;
        return &m_pInst;
    }

    CSoundManager();
    ~CSoundManager();

    void Init();
    void Release();

    void RegisterSound(std::string path, bool loop);
    std::vector<CSound>::iterator FindSound(std::string path);
    void PlaySound(std::string path);
    void PauseSound(std::string path);
    void ResumeSound(std::string path);
    void StopSound(std::string path);
    void VolumeUp(std::string path);
    void VolumeDown(std::string path);
    void UpdateSound(std::string path);
};