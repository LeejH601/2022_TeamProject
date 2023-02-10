#pragma once
#include "Sound.h"
#include <vector>
#include <algorithm>
#include <ctime>

class CSoundManager {
private:
    FMOD_SYSTEM* g_sound_system = nullptr;
    std::vector<CSound> m_Sounds;
    std::vector<std::string> m_vSoundPaths;

public:
    static CSoundManager* GetInst()
    {
        static std::shared_ptr<CSoundManager> m_pInst = nullptr;
        if (m_pInst.get() == nullptr) {
            m_pInst = std::make_shared<CSoundManager>();
            m_pInst->Init();
        }
        return m_pInst.get();
    }

    CSoundManager();
    ~CSoundManager();

    void Init();
    void Release();
    FMOD_SYSTEM* GetSoundSystem() { return g_sound_system; };
    std::vector<std::string>& GetAllSoundPaths();

    void RegisterSound(std::string path, bool loop);
    std::vector<CSound>::iterator FindSound(std::string path);
    std::vector<CSound>::iterator FindSound(unsigned int num);
    void PlaySound(std::string path);
    void PauseSound(std::string path);
    void ResumeSound(std::string path);
    void StopSound(std::string path);
    void VolumeUp(std::string path);
    void VolumeDown(std::string path);
    void UpdateSound(std::string path);
};