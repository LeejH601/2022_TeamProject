#include "Sound.h"

CSound::CSound()
{
}

CSound::CSound(FMOD_SYSTEM* g_sound_system, std::string path, bool loop)
{
	if (loop) {
		FMOD_System_CreateSound(g_sound_system, path.c_str(), FMOD_LOOP_NORMAL, 0, &m_sound);
	}
	else {
		FMOD_System_CreateSound(g_sound_system, path.c_str(), FMOD_DEFAULT, 0, &m_sound);
	}

	m_path = path;
	m_channel = nullptr;
	m_volume = SOUND_DEFAULT;
}

CSound::~CSound()
{
}

bool CSound::operator!=(const CSound& rhs)
{
	if (this->m_path != rhs.m_path)
		return true;
	return false;
}

CSound& CSound::operator=(const CSound& rhs)
{
	if (this->operator!=(rhs)) {
		this->m_sound = rhs.m_sound;
		this->m_bool = rhs.m_bool;
		this->m_channel = rhs.m_channel;
		this->m_path = rhs.m_path;
		this->m_volume = rhs.m_volume;
	}
	return *this;
}

void CSound::Release()
{
	FMOD_Sound_Release(m_sound);
}

void CSound::play(FMOD_SYSTEM* g_sound_system)
{
	FMOD_System_PlaySound(g_sound_system, m_sound, NULL, false, &m_channel);
	FMOD_Channel_SetVolume(m_channel, m_volume);
}

void CSound::pause()
{
	FMOD_Channel_SetPaused(m_channel, true);
}

void CSound::resume()
{
	FMOD_Channel_SetPaused(m_channel, false);
}

void CSound::stop() {
	FMOD_Channel_Stop(m_channel);
}

void CSound::volumeUp() {
	if (m_volume < SOUND_MAX) {
		m_volume += SOUND_WEIGHT;
	}

	FMOD_Channel_SetVolume(m_channel, m_volume);
}

void CSound::volumeDown() {
	if (m_volume > SOUND_MIN) {
		m_volume -= SOUND_WEIGHT;
	}

	FMOD_Channel_SetVolume(m_channel, m_volume);
}


void CSound::Update(FMOD_SYSTEM* g_sound_system) {
	FMOD_Channel_IsPlaying(m_channel, &m_bool);
	if (m_bool) {
		FMOD_System_Update(g_sound_system);
	}
}