#include "SoundManager.h"

CSoundManager::CSoundManager()
{
}

void CSoundManager::RegisterSound(std::string path, bool loop) {
    CSound sound = CSound(g_sound_system, path, loop);
    m_Sounds.push_back(sound);
}

std::vector<CSound>::iterator CSoundManager::FindSound(const std::string path)
{
    const std::vector<CSound>::iterator it = std::find_if(m_Sounds.begin(), m_Sounds.end(), [&](CSound sound) {
        if (sound.GetPath() == path)
        return true;
        });
    return it;
}

CSoundManager::~CSoundManager() {
}


void CSoundManager::Init() {
    FMOD_System_Create(&g_sound_system, FMOD_VERSION);
    FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, NULL);

    RegisterSound("Sound/David Bowie - Starman.mp3", false);
}

void CSoundManager::Release() {
    for (auto sound : m_Sounds)
        sound.Release();

    FMOD_System_Close(g_sound_system);
    FMOD_System_Release(g_sound_system);
}


void CSoundManager::PlaySound(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).play(g_sound_system);
}

void CSoundManager::PauseSound(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).pause();
}

void CSoundManager::ResumeSound(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).resume();
}

void CSoundManager::StopSound(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).stop();
}

void CSoundManager::VolumeUp(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).volumeUp();
}

void CSoundManager::VolumeDown(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).volumeDown();
}


void CSoundManager::UpdateSound(std::string path) {
    const std::vector<CSound>::iterator sound = FindSound(path);
    (*sound).Update(g_sound_system);
}
