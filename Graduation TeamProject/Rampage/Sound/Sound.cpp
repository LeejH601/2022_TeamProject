#include "Sound.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Global.h"
#include "..\Global\Locator.h"

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

void CSoundComponent::Update(float fElapsedTime)
{
	if (m_bEnable) {
		if (m_fCurrDelayed > m_fDelay)
			return;

		if (m_fCurrDelayed < m_fDelay) {
			m_fCurrDelayed += fElapsedTime;
		}

		if (m_fCurrDelayed >= m_fDelay) {
			if (m_pSound != nullptr) {
				m_pSound->play(g_sound_system);
			}
		}
	}
}

void CSoundComponent::Reset()
{
	m_fCurrDelayed = 0.0f;
	SetEnable(true);
}

void CSoundComponent::SetSound(CSound* sound)
{
	if (sound != nullptr) {

		m_pSound = new CSound();
		memcpy(m_pSound, sound, sizeof(CSound));
		m_fVolume = &m_pSound->m_volume;
	}
}


CEffectSoundComponent::CEffectSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CEffectSoundComponent::HandleMessage(const Telegram& msg)
{
	if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundEffect) {
		Reset();
		return true;
	}
	return false;
}

CShootSoundComponent::CShootSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CShootSoundComponent::HandleMessage(const Telegram& msg)
{
	if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundShoot) {
		Reset();
		return true;
	}
	return false;
}

CDamageSoundComponent::CDamageSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CDamageSoundComponent::HandleMessage(const Telegram& msg)
{
	if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundDamage) {
		Reset();
		return true;
	}
	return false;
}

void CSoundPlayer::Update(float fElapsedTime)
{
	if (m_pEffectComponent)
		m_pEffectComponent->Update(fElapsedTime);
	if (m_pDamageComponent)
		m_pDamageComponent->Update(fElapsedTime);
	if (m_pShootComponent)
		m_pShootComponent->Update(fElapsedTime);
}

void CSoundPlayer::LoadComponentFromSet(CComponentSet* componentset)
{
	m_pDamageComponent = componentset->FindComponent(typeid(CDamageSoundComponent));
	m_pEffectComponent = componentset->FindComponent(typeid(CEffectSoundComponent));
	m_pShootComponent = componentset->FindComponent(typeid(CShootSoundComponent));
}

bool CSoundPlayer::HandleMessage(const Telegram& msg)
{
	switch (static_cast<MESSAGE_TYPE>(msg.Msg))
	{
	case MESSAGE_TYPE::Msg_PlaySoundDamage:
		return m_pDamageComponent->HandleMessage(msg);
	case MESSAGE_TYPE::Msg_PlaySoundEffect:
		return m_pEffectComponent->HandleMessage(msg);
	case MESSAGE_TYPE::Msg_PlaySoundShoot:
		return m_pShootComponent->HandleMessage(msg);
	default:
		return false;
	}
}
