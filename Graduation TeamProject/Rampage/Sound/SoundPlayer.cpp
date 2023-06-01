#include "SoundPlayer.h"

void CSoundPlayer::PlaySound(FMOD_SYSTEM* g_sound_system, SoundPlayInfo sound_play_info, FMOD_CHANNELGROUP* channelgroup)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sound_play_info.fDelay * 1000)));
    sound_play_info.pSound->play(g_sound_system, channelgroup);

    /*TCHAR pstrDebug[256] = { 0 };
    _stprintf_s(pstrDebug, 256, _T("PlaySound\n"));
    OutputDebugString(pstrDebug);*/
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