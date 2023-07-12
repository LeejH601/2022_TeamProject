#include "SoundPlayer.h"

std::mutex vPlayingSoundsMutex;

void CSoundPlayer::PlaySound(FMOD_SYSTEM* g_sound_system, SoundPlayInfo sound_play_info, FMOD_CHANNELGROUP* channelgroup)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sound_play_info.fDelay * 1000)));

    std::lock_guard<std::mutex> lock(vPlayingSoundsMutex);

    for (auto it = m_vPlayingSounds.begin(); it != m_vPlayingSounds.end(); ++it) {
        FMOD_CHANNEL* pChannel = *it;

        FMOD_SOUND* currentSound;
        float fVolume;
        auto result = FMOD_Channel_GetCurrentSound(pChannel, &currentSound);

        if (currentSound == sound_play_info.pSound->GetSound())
        {
            //OutputDebugString(L"SameSound\n");
            FMOD_Channel_GetVolume(pChannel, &fVolume);
            FMOD_Channel_SetVolume(pChannel, fVolume * 0.8f);
        }
    }

    sound_play_info.pSound->play(g_sound_system, channelgroup);
    /*TCHAR pstrDebug[256] = { 0 };
    _stprintf_s(pstrDebug, 256, _T("PlaySound\n"));
    OutputDebugString(pstrDebug);*/
    m_vPlayingSounds.push_back(sound_play_info.pSound->GetChannel());
}

void CSoundPlayer::StopSound(FMOD_SOUND* pcurrentSound)
{
    for (auto it = m_vPlayingSounds.begin(); it != m_vPlayingSounds.end(); ++it) {
        FMOD_CHANNEL* pChannel = *it;

        FMOD_SOUND* currentSound;
        auto result = FMOD_Channel_GetCurrentSound(pChannel, &currentSound);

        if (currentSound == pcurrentSound)
        {
            FMOD_Channel_Stop(pChannel);
            return;
        }
    }
}

void CSoundPlayer::RegisterSound(CSound* pSound, float fDelay)
{
	SoundPlayInfo sound_play_info{ pSound, fDelay };
 
	m_vSounds.push_back(sound_play_info);
}

void CSoundPlayer::PlaySounds(FMOD_SYSTEM* g_sound_system, FMOD_CHANNELGROUP* channelgroup)
{
    for (auto it = m_vSounds.begin(); it != m_vSounds.end(); ) {
        std::thread sound_thread(&CSoundPlayer::PlaySound, this, g_sound_system, *it, channelgroup);
        sound_thread.detach();
        it = m_vSounds.erase(it);
    }
}

void CSoundPlayer::EraseAllPlayedSounds()
{
    std::lock_guard<std::mutex> lock(vPlayingSoundsMutex);

    auto it = m_vPlayingSounds.begin();
    for (auto it = m_vPlayingSounds.begin(); it != m_vPlayingSounds.end(); ) {
        FMOD_CHANNEL* pChannel = *it;

        FMOD_BOOL bIsPlaying;
        FMOD_Channel_IsPlaying(pChannel, &bIsPlaying);

        if (!bIsPlaying)
        {
            it = m_vPlayingSounds.erase(it);
            continue;
        }

        ++it;
    }
}
