#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\Global.h"
#include "Sound.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>

typedef std::map<std::string, std::pair<int, SOUND_CATEGORY>> SoundIndexMap;
typedef std::map<SOUND_CATEGORY, std::vector<CSound>> CategoryMap;

class CSoundManager {
private:
    FMOD_SYSTEM* g_sound_system = nullptr;
   /* std::vector<CSound> m_Sounds;
    std::vector<std::string> m_vSoundPaths;*/

    SoundIndexMap m_mSoundIndexMap;
    CategoryMap m_mCategoryMap;

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
    //std::vector<std::string>& GetAllSoundPaths();
    std::vector<std::string> getSoundPathsByCategory(SOUND_CATEGORY category);

    void RegisterSound(std::string path, bool loop, SOUND_CATEGORY category);
    std::vector<CSound>::iterator FindSound(std::string path);
    std::vector<CSound>::iterator FindSound(unsigned int num, SOUND_CATEGORY category);
    void PlaySound(std::string path);
    void PauseSound(std::string path);
    void ResumeSound(std::string path);
    void StopSound(std::string path);
    void VolumeUp(std::string path);
    void VolumeDown(std::string path);
    void UpdateSound(std::string path);
};