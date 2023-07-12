#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\Global.h"
#include "Sound.h"
#include "SoundPlayer.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>

typedef std::map<std::string, std::pair<int, SOUND_CATEGORY>> SoundIndexMap;
typedef std::map<SOUND_CATEGORY, std::vector<CSound>> CategoryMap;

class CSoundManager {
private:
    FMOD_SYSTEM* g_sound_system = nullptr;
    FMOD_CHANNELGROUP* m_channelgroup;
    CSoundPlayer m_SoundPlayer;

    SoundIndexMap m_mSoundIndexMap;
    CategoryMap m_mCategoryMap;

public:
    DECLARE_SINGLE(CSoundManager);

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

    void UpdateSound();
    void PlaySound(std::string path, float volume, float fDelay);
    void StopSound(std::string path);
};