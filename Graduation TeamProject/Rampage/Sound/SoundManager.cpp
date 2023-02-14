#include "SoundManager.h"

CSoundManager::CSoundManager()
{
}

void CSoundManager::RegisterSound(std::string path, bool loop, SOUND_CATEGORY category) {
	//CSound sound = CSound(g_sound_system, path, loop);

	int index = 0;
	CategoryMap::iterator it = m_mCategoryMap.find(category);
	if (it == m_mCategoryMap.end()) {
		std::pair<SOUND_CATEGORY, std::vector<CSound>> pair(category, std::vector<CSound>());
		pair.second.emplace_back(g_sound_system, path, loop);
		m_mCategoryMap.insert(pair);
	}
	else {
		index = it->second.size();
		it->second.emplace_back(g_sound_system, path, loop);
	}

	std::pair<int, SOUND_CATEGORY> pair(index, category);
	m_mSoundIndexMap[path] = pair;
	//m_mSoundIndexMap.emplace(path, (index, category));

	//m_Sounds.push_back(sound);
	//m_vSoundPaths.emplace_back(sound.GetPath());
}

std::vector<CSound>::iterator CSoundManager::FindSound(std::string path)
{
	SoundIndexMap::iterator it = m_mSoundIndexMap.find(path);

	if (it != m_mSoundIndexMap.end()) {
		CategoryMap::iterator ct_it = m_mCategoryMap.find(it->second.second);

		return ct_it->second.begin() + it->second.first;
	}

	/*const std::vector<CSound>::iterator it = std::find_if(m_Sounds.begin(), m_Sounds.end(), [path](CSound sound) {
		return sound.GetPath() == path;
		});
	return it;*/
}

std::vector<CSound>::iterator CSoundManager::FindSound(unsigned int num, SOUND_CATEGORY category)
{
	CategoryMap::iterator it = m_mCategoryMap.find(category);

	return it->second.begin() + num;

	//return FindSound(m_vSoundPaths[num]);
}

CSoundManager::~CSoundManager() {

	for (auto vec : m_mCategoryMap) {
		for (auto sound : vec.second) {
			sound.Release();
		}
	}

	/*for (auto sound : m_Sounds)
		sound.Release();*/
}


void CSoundManager::Init() {
	FMOD_System_Create(&g_sound_system, FMOD_VERSION);
	FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, NULL);
}

void CSoundManager::Release() {
	FMOD_System_Close(g_sound_system);
	FMOD_System_Release(g_sound_system);
}

std::vector<std::string> CSoundManager::getSoundPathsByCategory(SOUND_CATEGORY category)
{
	CategoryMap::iterator it = m_mCategoryMap.find(category);
	if (it != m_mCategoryMap.end()) {
		std::vector<std::string> paths;
		for (auto& sound : it->second) {
			paths.emplace_back(sound.GetPath());
		}

		return paths;
	}

	return std::vector<std::string>();
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
