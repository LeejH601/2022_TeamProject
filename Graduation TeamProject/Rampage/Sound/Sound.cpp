#include "Sound.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Global.h"
#include "..\Global\Locator.h"
#include "..\Global\Timer.h"
#include "SoundManager.h"

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
		if(m_pSound)
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
	case MESSAGE_TYPE::Msg_SoundDamageReady:
		return m_pDamageComponent->HandleMessage(msg);
	case MESSAGE_TYPE::Msg_SoundEffectReady:
		return m_pEffectComponent->HandleMessage(msg);
	case MESSAGE_TYPE::Msg_SoundShootReady:
		return m_pShootComponent->HandleMessage(msg);
	default:
		return false;
	}
}
