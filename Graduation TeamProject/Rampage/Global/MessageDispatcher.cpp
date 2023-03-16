#include "MessageDispatcher.h"
#include "Camera.h"
#include "..\Object\Object.h"
#include "..\Sound\SoundManager.h"

void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject)
{
	ListenerInfo info = { listener, filterObject };
	m_listeners[static_cast<int>(messageType)].push_back(info);
}

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
void CameraShakeComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void CameraZoomerComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable || !pCamera->m_bCameraZooming)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	float ZoomConstant = (m_bIsIN) ? 1.0f : -1.0f;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length * ZoomConstant, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		pCamera->m_bCameraZooming = false;
		m_fCurrDistance = 0.f;
		offset.x = 0.0f;
		offset.y = 0.0f;
		offset.z = 0.0f;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length * ZoomConstant, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraZoomerComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void CameraMoveComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable || !pCamera->m_bCameraMoving)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		pCamera->m_bCameraMoving = false;
		m_fCurrDistance = 0.f;
		offset.x = 0.0f;
		offset.y = 0.0f;
		offset.z = 0.0f;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}

void CameraMoveComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
