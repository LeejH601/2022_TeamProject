#include "..\Global\Global.h"
#include "SoundComponent.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Sound\SoundManager.h"
#include "..\Global\Locator.h"
#include "..\Global\Timer.h"

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

			}
		}
	}
}

void CSoundComponent::Reset()
{
	m_fCurrDelayed = 0.0f;
	UpdateVolume();
	//SetSound(&*(CSoundManager::GetInst()->FindSound(m_nSoundNumber)));
}

void CSoundComponent::SetSound(CSound* sound)
{
	if (sound != nullptr) {
		/*if (m_pSound)
			delete m_pSound;

		m_pSound = new CSound();
		memcpy(m_pSound, sound, sizeof(CSound));*/
		m_pSound = sound;
	}
}


CEffectSoundComponent::CEffectSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CEffectSoundComponent::HandleMessage(const Telegram& msg)
{
	if (!GetEnable())
		return false;

	if (msg.Msg == (int)MESSAGE_TYPE::Msg_SoundEffectReady) {
		Reset();
		Telegram Msg;
		Msg.Receiver = msg.Receiver;
		Msg.Msg = (int)MESSAGE_TYPE::Msg_PlaySoundEffect;
		Msg.DispatchTime = Locator.GetTimer()->GetNowTimeAfter(m_fDelay);
		Locator.GetMessageDispather()->RegisterMessage(Msg);
		return true;
	}
	else if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundEffect) {
		if (m_pSound)
			m_pSound->play(g_sound_system);
		return true;
	}
	return false;
}

void CEffectSoundComponent::Reset()
{
	CSoundComponent::Reset();
	SetSound(&*(CSoundManager::GetInst()->FindSound(m_nSoundNumber, SOUND_CATEGORY::SOUND_SHOCK)));
}

CShootSoundComponent::CShootSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CShootSoundComponent::HandleMessage(const Telegram& msg)
{
	if (!GetEnable())
		return false;

	if (msg.Msg == (int)MESSAGE_TYPE::Msg_SoundShootReady) {
		Reset();
		Telegram Msg;
		Msg.Receiver = msg.Receiver;
		Msg.Msg = (int)MESSAGE_TYPE::Msg_PlaySoundShoot;
		Msg.DispatchTime = Locator.GetTimer()->GetNowTimeAfter(m_fDelay);
		Locator.GetMessageDispather()->RegisterMessage(Msg);
		return true;
	}
	else if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundShoot) {
		if (m_pSound)
			m_pSound->play(g_sound_system);
		return true;
	}
	return false;
}

void CShootSoundComponent::Reset()
{
	CSoundComponent::Reset();
	SetSound(&*(CSoundManager::GetInst()->FindSound(m_nSoundNumber, SOUND_CATEGORY::SOUND_SHOOT)));
}

CDamageSoundComponent::CDamageSoundComponent(FMOD_SYSTEM* sound_system)
{
	g_sound_system = sound_system;
}

bool CDamageSoundComponent::HandleMessage(const Telegram& msg)
{
	if (!GetEnable())
		return false;

	if (msg.Msg == (int)MESSAGE_TYPE::Msg_SoundDamageReady) {
		Reset();
		Telegram Msg;
		Msg.Receiver = msg.Receiver;
		Msg.Msg = (int)MESSAGE_TYPE::Msg_PlaySoundDamage;
		Msg.DispatchTime = Locator.GetTimer()->GetNowTimeAfter(m_fDelay);
		Locator.GetMessageDispather()->RegisterMessage(Msg);
		return true;
	}
	else if (msg.Msg == (int)MESSAGE_TYPE::Msg_PlaySoundDamage) {
		if (m_pSound)
			m_pSound->play(g_sound_system);
		return true;
	}
	return false;
}

void CDamageSoundComponent::Reset()
{
	CSoundComponent::Reset();
	SetSound(&*(CSoundManager::GetInst()->FindSound(m_nSoundNumber, SOUND_CATEGORY::SOUND_VOICE)));
}

