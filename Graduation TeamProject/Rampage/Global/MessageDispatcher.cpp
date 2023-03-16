#include "MessageDispatcher.h"
#include "Camera.h"
#include "..\Object\Object.h"
#include "..\Sound\SoundManager.h"

void PlayerAttackComponent::HandleMessage(const Message& message, const PlayerAttackParams& params)
{
    if (message.getType() == MessageType::PLAYER_ATTACK) {
        params.pPlayer->CheckCollision(m_pObject);
    }
}
void SoundPlayComponent::HandleMessage(const Message& message, const SoundPlayParams& params)
{
    if (message.getType() == MessageType::PLAY_SOUND && m_sc == params.sound_category) {
        auto pSound = CSoundManager::GetInst()->FindSound(m_nSoundNumber, m_sc);
        CSoundManager::GetInst()->PlaySound(pSound->GetPath(), m_fVolume, m_fDelay);
    }
}
void CameraShakeComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable)
		return;
	if (pCamera->m_bCameraShaking && m_fDuration > m_ft) {
		m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = pCamera->GetRightVector();

		float ShakeConstant = urd(dre);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, ShakeConstant * m_fMagnitude, false);
		XMMATRIX RotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&pCamera->GetLookVector()), RotateConstant);

		XMFLOAT3 ShakeOffset; XMStoreFloat3(&ShakeOffset, XMVector3TransformCoord(XMLoadFloat3(&CameraDir), RotateMatrix));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, ShakeOffset);
	}
	else {
		m_ft = 0.0f;
		pCamera->m_bCameraShaking = false;
	}
}
void CameraShakeComponent::HandleMessage(const Message& message, const CameraShakeParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject)
{
    ListenerInfo info = { listener, filterObject };
    m_listeners[static_cast<int>(messageType)].push_back(info);
}
/*
플레이어 - 발사 사운드
몬스터 - 데미지 사운드, 충격 사운드
어떤 몬스터 맞았는지?에 대한 정보
*/