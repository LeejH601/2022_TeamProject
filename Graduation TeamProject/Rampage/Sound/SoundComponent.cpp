#include "..\Global\Global.h"
#include "SoundComponent.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Sound\SoundManager.h"

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
				m_bEnable = false;
			}
		}
	}
}

void CSoundComponent::Reset()
{
	m_fCurrDelayed = 0.0f;
	SetEnable(true);
}

void CSoundComponent::SetSound(const std::string path)
{
	CSound* sound = &(*CSoundManager::GetInst()->FindSound("Sound/David Bowie - Starman.mp3"));
	m_pSound = sound;
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