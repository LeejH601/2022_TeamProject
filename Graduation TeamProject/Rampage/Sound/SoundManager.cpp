#include "SoundManager.h"

CSoundManager::CSoundManager()
{
}

void CSoundManager::RegisterSound(std::string path, bool loop) {
	CSound sound = CSound(g_sound_system, path, loop);
	m_Sounds.push_back(sound);
	m_vSoundPaths.emplace_back(sound.GetPath());
}

std::vector<CSound>::iterator CSoundManager::FindSound(std::string path)
{
	const std::vector<CSound>::iterator it = std::find_if(m_Sounds.begin(), m_Sounds.end(), [path](CSound sound) {
		return sound.GetPath() == path;
		});
	return it;
}

std::vector<CSound>::iterator CSoundManager::FindSound(unsigned int num)
{
	return FindSound(m_vSoundPaths[num]);
}

CSoundManager::~CSoundManager() {
	for (auto sound : m_Sounds)
		sound.Release();
}


void CSoundManager::Init() {
	FMOD_System_Create(&g_sound_system, FMOD_VERSION);
	FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, NULL);
}

void CSoundManager::Release() {
	FMOD_System_Close(g_sound_system);
	FMOD_System_Release(g_sound_system);
}

std::vector<std::string>& CSoundManager::GetAllSoundPaths()
{
	return m_vSoundPaths;
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
