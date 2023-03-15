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
        // ��Ե� �����̿� ���缭 ����϶�~
    }
}
/*
�÷��̾� - �߻� ����
���� - ������ ����, ��� ����
� ���� �¾Ҵ���?�� ���� ����
*/