#include "MessageDispatcher.h"
#include "..\Object\Object.h"
#include "..\Sound\SoundManager.h"

void PlayerAttackComponent::HandleMessage(const Message& message, const PlayerAttackParams& params)
{
    if (message.getType() == MessageType::PLAYER_ATTACK) {
        params.pPlayer->CheckCollision(m_pObject);
    }
}

void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener)
{
	m_listeners[(int)messageType].push_back(listener);
}

void SoundPlayComponent::HandleMessage(const Message& message, const SoundPlayParams& params)
{
    if (message.getType() == MessageType::PLAY_SOUND) {
        CSoundManager::GetInst()->PlaySound("Sound/Air Cut by Langerium Id-84616.wav");
        // 어떻게든 딜레이에 맞춰서 출력하라~
    }
}
/*
플레이어 - 발사 사운드
몬스터 - 데미지 사운드, 충격 사운드
어떤 몬스터 맞았는지?에 대한 정보
*/