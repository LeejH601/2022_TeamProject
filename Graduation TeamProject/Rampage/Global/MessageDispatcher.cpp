#include "MessageDispatcher.h"
#include "..\Object\Object.h"
#include "..\Sound\SoundManager.h"

void PlayerAttackComponent::HandleMessage(const Message& message, const PlayerAttackParams& params)
{
    if (message.getType() == MessageType::PLAYER_ATTACK) {
        params.pPlayer->CheckCollision(m_pObject);
    }
}

void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject)
{
    ListenerInfo info = { listener, filterObject };
    m_listeners[static_cast<int>(messageType)].push_back(info);
}

void SoundPlayComponent::HandleMessage(const Message& message, const SoundPlayParams& params)
{
    if (message.getType() == MessageType::PLAY_SOUND && m_spt == params.spt) {
        auto pSound = CSoundManager::GetInst()->FindSound(m_nSoundNumber, m_sc);
        CSoundManager::GetInst()->PlaySound(pSound->GetPath(), m_fVolume, m_fDelay);
    }
}
/*
플레이어 - 발사 사운드
몬스터 - 데미지 사운드, 충격 사운드
어떤 몬스터 맞았는지?에 대한 정보
*/